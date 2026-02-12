#include "main.hpp"
#include <zmq.hpp>
#include <fstream>
#include <nlohmann/json.hpp> 

using json = nlohmann::json;


    
static void save_as_json_array(const string& new_json) {
    static mutex file_mutex;
    lock_guard<mutex> lock(file_mutex);
    
    // Открываем файл в режиме append (добавление в конец)
    ofstream file("location_data.jsonl", ios::app);
    
    if (file.is_open()) {
        // Добавляем новую JSON строку и переводим строку
        file << new_json << "\n";
        file.close();
        cout << "[SERVER] Location saved to location_data.jsonl" << endl;
    } else {
        cerr << "[SERVER] Error: Could not open location_data.jsonl for writing" << endl;
    }
}


// Упрощенная функция для извлечения float из JSON
float extract_float_from_json(const json& j, const string& key) {
    if (j.contains(key) && !j[key].is_null()) {
        return j[key].get<float>();
    }
    return 0.0f;
}

void run_server(location *loc) {  
    cout << "[SERVER] Starting server thread..." << endl;
    
    try {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::rep);

        socket.set(zmq::sockopt::linger, 0);  // Немедленно закрывать
        
        socket.bind("tcp://*:4789");
        cout << "[SERVER] Listening on tcp://*:4789" << endl;
        
        
        while (!loc->server_stop) {
            zmq::message_t request;
            
            zmq::pollitem_t items[] = {{socket, 0, ZMQ_POLLIN, 0}};
            zmq::poll(items, 1, 100);
            
            if (items[0].revents & ZMQ_POLLIN) {
                socket.recv(request, zmq::recv_flags::none);
                
                std::string message(static_cast<char*>(request.data()), request.size());
                std::cout << "[SERVER] Received: " << message << std::endl;
                
                try {
                    json j = json::parse(message);
                    
                    // Сохраняем сырые данные
                    save_as_json_array(message);
                    
                    // Извлекаем данные
                    lock_guard<mutex> lock(loc->location_mutex);
                    loc->latitude = j.value("latitude", 0.0f);
                    loc->longitude = j.value("longitude", 0.0f);
                    loc->altitude = j.value("altitude", 0.0f);
                    loc->accuracy = j.value("accuracy", 0.0f);
                    loc->timestamp = std::chrono::system_clock::now();
                    loc->new_data = true;
                    
                    cout << "[SERVER] Parsed: " << loc->latitude 
                              << ", " << loc->longitude << endl;
                    
                    // Отправляем ответ
                    string response = "Локация получена";
                    zmq::message_t reply(response.size());
                    memcpy(reply.data(), response.c_str(), response.size());
                    socket.send(reply, zmq::send_flags::none);
                    
                } catch (const json::parse_error& e) {
                    cerr << "[SERVER] JSON parse error: " << e.what() << endl;
                }
            }
        }
        
        socket.close();
        context.close();
        
    } catch (const exception& e) {
        cerr << "[SERVER] Error: " << e.what() << endl;
    }
    
    cout << "[SERVER] Server thread finished" << endl;
}