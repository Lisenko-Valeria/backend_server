#pragma once

#include <string>
#include <atomic>
#include <chrono>
#include <mutex>
#include <iostream>
#include <thread>
#include<vector>
using namespace std;

struct location {  
    string readable_time;
    float latitude = 0.0;   
    float longitude = 0.0;  
    float altitude = 0.0;    
    float accuracy = 0.0;   

    int cell_count = 0;
    vector<string> cell_types;
    vector<string> cell_band;
    vector<int> cell_ci;
    vector<int> cell_earfcn;
    vector<string> cell_mcc;
    vector<string> cell_mnc;
    vector<int> cell_pci;
    vector<int> cell_tac;
    vector<int> cell_asu_level;
    vector<int> cell_cqi;
    vector<int> cell_rsrp;
    vector<int> cell_rsrq;
    vector<int> cell_rssi;
    vector<int> cell_rssnr;
    vector<int> cell_timing_advance;

    chrono::system_clock::time_point timestamp;
    atomic<bool> new_data{false}; 
    atomic<bool> server_stop{false};

    mutex location_mutex; 
};

void run_gui(location *loc);    
void run_server(location *loc); 
