#include "main.hpp"

int main() {

    static location locationInfo;

    thread gui_thread(run_gui, &locationInfo);
    thread server_thread(run_server, &locationInfo);
    
    gui_thread.join();
    server_thread.join();
    return 0;
}