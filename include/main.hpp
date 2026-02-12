#pragma once

#include <string>
#include <atomic>
#include <chrono>
#include <mutex>
#include <iostream>
#include <thread>
using namespace std;

struct location {  
    float latitude = 0.0;   
    float longitude = 0.0;  
    float altitude = 0.0;    
    float accuracy = 0.0;   
    chrono::system_clock::time_point timestamp;
    atomic<bool> new_data{false}; 
    atomic<bool> server_stop{false};

    mutex location_mutex; 
};

void run_gui(location *loc);    
void run_server(location *loc); 
