#include "main.hpp"
#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

void run_gui(location *loc) {  
    cout << "[GUI] Starting GUI thread..." << endl;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Location Server",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1400, 900, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); 
    
    glewExperimental = GL_TRUE;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    
    ImGui::StyleColorsLight();
    
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    bool running = true;
    
        
    float x_values[MAX_HISTORY] = {0};

    float asu_level_values[MAX_HISTORY] = {0};
    float cqi_values[MAX_HISTORY] = {0};
    float rsrp_values[MAX_HISTORY] = {0};
    float rsrq_values[MAX_HISTORY] = {0};
    float rssi_values[MAX_HISTORY] = {0};
    float rssnr_values[MAX_HISTORY] = {0};
    float timing_advance_values[MAX_HISTORY] = {0};
    
    int current_cell_index = 0;
    
    bool first_data_received = false;
    
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            
            if (event.type == SDL_QUIT ||
               (event.type == SDL_WINDOWEVENT && 
                event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                running = false;
                loc->server_stop = true;
            }
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (loc->new_signal_data) {
            lock_guard<mutex> lock(loc->location_mutex);

            if (!first_data_received && loc->cell_count > 0) {
                first_data_received = true;
        
                for (int i = 0; i < MAX_HISTORY; i++) {
                    asu_level_values[i] = 0;
                    cqi_values[i] = 0;
                    rsrp_values[i] = 0;
                    rsrq_values[i] = 0;
                    rssi_values[i] = 0;
                    rssnr_values[i] = 0;
                    timing_advance_values[i] = 0;
                    x_values[i] = i;
                }
            }
            
            if (loc->cell_count > 0) {
                current_cell_index = 0;
                
                for (int i = 0; i < MAX_HISTORY; i++) {
                    x_values[i] = i;
                }
        
                if (loc->cell_asu_level.size() > current_cell_index) {
                    for (int i = 0; i < MAX_HISTORY - 1; i++) {
                        asu_level_values[i] = asu_level_values[i + 1];
                    }
                    asu_level_values[MAX_HISTORY - 1] = loc->cell_asu_level[current_cell_index];
                }
                
                if (loc->cell_cqi.size() > current_cell_index) {
                    for (int i = 0; i < MAX_HISTORY - 1; i++) {
                        cqi_values[i] = cqi_values[i + 1];
                    }
                    cqi_values[MAX_HISTORY - 1] = loc->cell_cqi[current_cell_index];
                }
                
                if (loc->cell_rsrp.size() > current_cell_index) {
                    for (int i = 0; i < MAX_HISTORY - 1; i++) {
                        rsrp_values[i] = rsrp_values[i + 1];
                    }
                    rsrp_values[MAX_HISTORY - 1] = loc->cell_rsrp[current_cell_index];
                }
                
                if (loc->cell_rsrq.size() > current_cell_index) {
                    for (int i = 0; i < MAX_HISTORY - 1; i++) {
                        rsrq_values[i] = rsrq_values[i + 1];
                    }
                    rsrq_values[MAX_HISTORY - 1] = loc->cell_rsrq[current_cell_index];
                }
                
                if (loc->cell_rssi.size() > current_cell_index) {
                    for (int i = 0; i < MAX_HISTORY - 1; i++) {
                        rssi_values[i] = rssi_values[i + 1];
                    }
                    rssi_values[MAX_HISTORY - 1] = loc->cell_rssi[current_cell_index];
                }
                
                if (loc->cell_rssnr.size() > current_cell_index) {
                    for (int i = 0; i < MAX_HISTORY - 1; i++) {
                        rssnr_values[i] = rssnr_values[i + 1];
                    }
                    rssnr_values[MAX_HISTORY - 1] = loc->cell_rssnr[current_cell_index];
                }
                
                if (loc->cell_timing_advance.size() > current_cell_index) {
                    for (int i = 0; i < MAX_HISTORY - 1; i++) {
                        timing_advance_values[i] = timing_advance_values[i + 1];
                    }
                    timing_advance_values[MAX_HISTORY - 1] = loc->cell_timing_advance[current_cell_index];
                }
            }
            
            loc->new_signal_data = false;
        }
        
        ImGui::Begin("Location Server", nullptr, ImGuiWindowFlags_NoCollapse);
        
        if (ImGui::CollapsingHeader("Location Data", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.0f, 0.5f, 1.0f));  
            if (loc->latitude != 0 || loc->longitude != 0) {
                ImGui::Text("Latitude:  %.8f°", loc->latitude);
                ImGui::Text("Longitude: %.8f°", loc->longitude);
                ImGui::Text("Altitude:  %.2f m", loc->altitude);
                ImGui::Text("Accuracy:  ±%.2f m", loc->accuracy);
                
                auto time = std::chrono::system_clock::to_time_t(loc->timestamp);
                char time_str[100];
                std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
                ImGui::Text("Last update: %s", time_str); 
            } else {
                ImGui::Text("Waiting for data..."); 
                ImGui::Text("Server is running on port 4789");  
            }
            ImGui::PopStyleColor(1);
        }
        
        ImGui::Separator();
        
        if (ImGui::CollapsingHeader("Signal Strength Graphs", ImGuiTreeNodeFlags_DefaultOpen)) {
            
            ImGui::Columns(2, "signal_columns", false);
            
            if (ImPlot::BeginPlot("RSRP (dBm)", ImVec2(-1, 200))) {
                ImPlot::SetupAxes("Samples", "dBm");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_HISTORY - 1);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -140, -40);
                ImPlot::SetNextLineStyle(ImVec4(1,0,0,1), 2);
                ImPlot::PlotLine("RSRP", x_values, rsrp_values, MAX_HISTORY);
                ImPlot::EndPlot();
            }
            
            ImGui::NextColumn();
            
            if (ImPlot::BeginPlot("RSRQ (dB)", ImVec2(-1, 200))) {
                ImPlot::SetupAxes("Samples", "dB");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_HISTORY - 1);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -20, -3);
                ImPlot::SetNextLineStyle(ImVec4(0,1,0,1), 2);
                ImPlot::PlotLine("RSRQ", x_values, rsrq_values, MAX_HISTORY);
                ImPlot::EndPlot();
            }
            
            ImGui::Columns(1);
            ImGui::Separator();
            
            ImGui::Columns(2, "signal_columns2", false);
            
            if (ImPlot::BeginPlot("RSSNR (dB)", ImVec2(-1, 200))) {
                ImPlot::SetupAxes("Samples", "dB");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_HISTORY - 1);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -10, 30);
                ImPlot::SetNextLineStyle(ImVec4(0,0,1,1), 2);
                ImPlot::PlotLine("RSSNR", x_values, rssnr_values, MAX_HISTORY);
                ImPlot::EndPlot();
            }
            
            ImGui::NextColumn();
            
            if (ImPlot::BeginPlot("CQI", ImVec2(-1, 200))) {
                ImPlot::SetupAxes("Samples", "Value");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_HISTORY - 1);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 15);
                ImPlot::SetNextLineStyle(ImVec4(1,0.5f,0,1), 2);
                ImPlot::PlotLine("CQI", x_values, cqi_values, MAX_HISTORY);
                ImPlot::EndPlot();
            }
            
            ImGui::Columns(1);
            ImGui::Separator();
            
            ImGui::Columns(2, "signal_columns3", false);
            
            if (ImPlot::BeginPlot("ASU Level", ImVec2(-1, 200))) {
                ImPlot::SetupAxes("Samples", "Level");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_HISTORY - 1);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);
                ImPlot::SetNextLineStyle(ImVec4(0.5f,0,0.5f,1), 2);
                ImPlot::PlotLine("ASU", x_values, asu_level_values, MAX_HISTORY);
                ImPlot::EndPlot();
            }
            
            ImGui::NextColumn();
            
            if (ImPlot::BeginPlot("RSSI", ImVec2(-1, 200))) {
                ImPlot::SetupAxes("Samples", "Value");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_HISTORY - 1);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -120, -30);
                ImPlot::SetNextLineStyle(ImVec4(0,1,1,1), 2);
                ImPlot::PlotLine("RSSI", x_values, rssi_values, MAX_HISTORY);
                ImPlot::EndPlot();
            }
            
            ImGui::Columns(1);
            ImGui::Separator();
            
            if (ImPlot::BeginPlot("Timing Advance", ImVec2(-1, 200))) {
                ImPlot::SetupAxes("Samples", "TA");
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_HISTORY - 1);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);
                ImPlot::SetNextLineStyle(ImVec4(1,1,0,1), 2);
                ImPlot::PlotLine("TA", x_values, timing_advance_values, MAX_HISTORY);
                ImPlot::EndPlot();
            }
        }
        
        ImGui::Separator();
        
        if (ImGui::CollapsingHeader("All Cells Information", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Cells count: %d", loc->cell_count);
            
            if (loc->cell_count > 0) {
                ImGui::Separator();
                
                for (int i = 0; i < loc->cell_count; i++) {
                    string header = "Cell " + to_string(i+1) + ' ' + 
                                    (i < loc->cell_types.size() ? loc->cell_types[i] : "Unknown");
                    
                    if (ImGui::TreeNode(header.c_str())) {
                        if (i < loc->cell_band.size())
                            ImGui::Text("Band: %s", loc->cell_band[i].c_str());
                        if (i < loc->cell_ci.size())
                            ImGui::Text("CI: %d", loc->cell_ci[i]);
                        if (i < loc->cell_earfcn.size())
                            ImGui::Text("EARFCN: %d", loc->cell_earfcn[i]);
                        if (i < loc->cell_mcc.size())
                            ImGui::Text("MCC: %s", loc->cell_mcc[i].c_str());
                        if (i < loc->cell_mnc.size())
                            ImGui::Text("MNC: %s", loc->cell_mnc[i].c_str());
                        if (i < loc->cell_pci.size())
                            ImGui::Text("PCI: %d", loc->cell_pci[i]);
                        if (i < loc->cell_tac.size())
                            ImGui::Text("TAC: %d", loc->cell_tac[i]);
                        
                        ImGui::Separator();
                        ImGui::Text("Signal Parameters:");
                        
                        if (i < loc->cell_asu_level.size())
                            ImGui::Text("ASU Level: %d", loc->cell_asu_level[i]);
                        if (i < loc->cell_cqi.size())
                            ImGui::Text("CQI: %d", loc->cell_cqi[i]);
                        if (i < loc->cell_rsrp.size())
                            ImGui::Text("RSRP: %d dBm", loc->cell_rsrp[i]);
                        if (i < loc->cell_rsrq.size())
                            ImGui::Text("RSRQ: %d dB", loc->cell_rsrq[i]);
                        if (i < loc->cell_rssi.size())
                            ImGui::Text("RSSI: %d", loc->cell_rssi[i]);
                        if (i < loc->cell_rssnr.size())
                            ImGui::Text("RSSNR: %d dB", loc->cell_rssnr[i]);
                        if (i < loc->cell_timing_advance.size())
                            ImGui::Text("Timing Advance: %d", loc->cell_timing_advance[i]);
                        
                        ImGui::TreePop();
                    }
                    ImGui::Separator();
                }
            } else {
                ImGui::Text("No cell data available");
            }
        }
        
        ImGui::Separator();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.8f, 1.0f));       
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));         
        
        if (ImGui::Button("Exit")) {
            running = false;
            loc->server_stop = true;
        }
        
        ImGui::PopStyleColor(2); 
        ImGui::End();
        
        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        
        this_thread::sleep_for(std::chrono::milliseconds(8));
    }
    
    ImPlot::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    cout << "[GUI] GUI thread finished" << endl;
}