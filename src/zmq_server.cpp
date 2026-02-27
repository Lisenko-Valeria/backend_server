#include "main.hpp"
#include <zmq.hpp>
#include <fstream>
#include <nlohmann/json.hpp> 

using json = nlohmann::json;


    
static void save_as_json_array(const string& new_json) {
    static mutex file_mutex;
    lock_guard<mutex> lock(file_mutex);
    
    ofstream file("location_data.jsonl", ios::app);
    
    if (file.is_open()) {
        file << new_json << "\n";
        file.close();
        cout << "[SERVER] Location saved to location_data.jsonl" << endl;
    } else {
        cerr << "[SERVER] Error: Could not open location_data.jsonl for writing" << endl;
    }
}


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

        socket.set(zmq::sockopt::linger, 0);  
        
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
                    
                    save_as_json_array(message);
                    
                    //////////////////////////////////////////
                    lock_guard<mutex> lock(loc->location_mutex);

                    if (j.contains("readable_time")) {
                        loc->readable_time = j["readable_time"].get<string>();}

                    
                    if (j.contains("location") && !j["location"].is_null()) {
                        auto& loc_json = j["location"];
                        loc->latitude = loc_json.value("latitude", 0.0f);
                        loc->longitude = loc_json.value("longitude", 0.0f);
                        loc->altitude = loc_json.value("altitude", 0.0f);
                        loc->accuracy = loc_json.value("accuracy", 0.0f);
                    } else {
                        loc->latitude = 0.0f;
                        loc->longitude = 0.0f;
                        loc->altitude = 0.0f;
                        loc->accuracy = 0.0f;
                    }

                    loc->cell_types.clear();
                    loc->cell_band.clear();
                    loc->cell_ci.clear();
                    loc->cell_earfcn.clear();
                    loc->cell_mcc.clear();
                    loc->cell_mnc.clear();
                    loc->cell_pci.clear();
                    loc->cell_tac.clear();
                    loc->cell_asu_level.clear();
                    loc->cell_cqi.clear();
                    loc->cell_rsrp.clear();
                    loc->cell_rsrq.clear();
                    loc->cell_rssi.clear();
                    loc->cell_rssnr.clear();
                    loc->cell_timing_advance.clear();


                    if (j.contains("cell_info") && j["cell_info"].contains("cells")) {
                        auto& cells = j["cell_info"]["cells"];
                        loc->cell_count = cells.size();
                        
                        for (auto& cell : cells) {
                            loc->cell_types.push_back(cell.value("type", "Unknown"));
                            
                            if (cell.contains("identity")) {
                                auto& id = cell["identity"];
                                loc->cell_band.push_back(id.value("band", "N/A"));
                                loc->cell_ci.push_back(id.value("ci", 0));
                                loc->cell_earfcn.push_back(id.value("earfcn", 0));
                                loc->cell_mcc.push_back(id.value("mcc", "N/A"));
                                loc->cell_mnc.push_back(id.value("mnc", "N/A"));
                                loc->cell_pci.push_back(id.value("pci", 0));
                                loc->cell_tac.push_back(id.value("tac", 0));
                            } else {
                                loc->cell_band.push_back("N/A");
                                loc->cell_ci.push_back(0);
                                loc->cell_earfcn.push_back(0);
                                loc->cell_mcc.push_back("N/A");
                                loc->cell_mnc.push_back("N/A");
                                loc->cell_pci.push_back(0);
                                loc->cell_tac.push_back(0);
                            }
                            if (cell.contains("signal")) {
                                auto& sig = cell["signal"];
                                loc->cell_asu_level.push_back(sig.value("asu_level", 0));
                                loc->cell_cqi.push_back(sig.value("cqi", 0));
                                loc->cell_rsrp.push_back(sig.value("rsrp", 0));
                                loc->cell_rsrq.push_back(sig.value("rsrq", 0));
                                loc->cell_rssi.push_back(sig.value("rssi", 0));
                                loc->cell_rssnr.push_back(sig.value("rssnr", 0));
                                loc->cell_timing_advance.push_back(sig.value("timing_advance", 0));
                            } else {
                                loc->cell_asu_level.push_back(0);
                                loc->cell_cqi.push_back(0);
                                loc->cell_rsrp.push_back(0);
                                loc->cell_rsrq.push_back(0);
                                loc->cell_rssi.push_back(0);
                                loc->cell_rssnr.push_back(0);
                                loc->cell_timing_advance.push_back(0);
                            }
                        }
                    }

                    loc->timestamp = std::chrono::system_clock::now();
                    loc->new_data = true;
                    ///////////////////////////////////////////
                    cout << "[SERVER] Parsed: " << loc->latitude 
                              << ", " << loc->longitude << endl;
                    
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