#pragma once

#include <string>
#include <atomic>
#include <chrono>
#include <mutex>
#include <iostream>
#include <thread>
#include<vector>
using namespace std;

const int MAX_HISTORY = 100; 

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

    vector<float> asu_level_history;      
    vector<float> cqi_history;            
    vector<float> rsrp_history;          
    vector<float> rsrq_history;            
    vector<float> rssi_history;           
    vector<float> rssnr_history;           
    vector<float> timing_advance_history;  
    
    float current_asu_level = -999;
    float current_cqi = -999;
    float current_rsrp = -999;
    float current_rsrq = -999;
    float current_rssi = -999;
    float current_rssnr = -999;
    float current_timing_advance = -999;
    
    atomic<bool> new_signal_data{false};
};

void run_gui(location *loc);    
void run_server(location *loc); 
