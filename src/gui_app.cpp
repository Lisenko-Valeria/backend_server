#include "main.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>

void run_gui(location *loc) {  
    cout << "[GUI] Starting GUI thread..." << endl;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Location Server",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); 
    
    glewExperimental = GL_TRUE;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    bool running = true;
    
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
        
        ImGui::Begin("Location Server", nullptr, ImGuiWindowFlags_NoCollapse);
        
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
        ImGui::Separator();
        

            ImGui::Text("Cells count: %d", loc->cell_count);
            
            if (loc->cell_count > 0) {
                ImGui::Separator();
                
                for (int i = 0; i < loc->cell_count; i++) {
                    string header = "Cell " + to_string(i+1) +' '+ loc->cell_types[i];
                    
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
        
        this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    cout << "[GUI] GUI thread finished" << endl;
}