#include "main.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>

void run_gui(location *loc) {  
    cout << "[GUI] Starting GUI thread..." << endl;
    
    // Инициализация SDL
    SDL_Init(SDL_INIT_VIDEO);
    

        // Создание окна
    SDL_Window* window = SDL_CreateWindow("Location Server",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        600, 400, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        
    // Контекст OpenGL 
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); //частота

    
    // Инициализация ImGui
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    
    // Привязка Imgui к SDL2 и OpenGl backend'ам
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    bool running = true;
    
    // Главный цикл GUI
    while (running) {
        // Обработка событий
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
        
        // Новый фрейм ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        









        // Главное окно
        ImGui::Begin("Location Server", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.0f, 0.5f, 1.0f));  
        if (loc->latitude != 0 || loc->longitude != 0) {
            ImGui::Text("Latitude:  %.8f°", loc->latitude);
            ImGui::Text("Longitude: %.8f°", loc->longitude);
            ImGui::Text("Altitude:  %.2f m", loc->altitude);
            ImGui::Text("Accuracy:  ±%.2f m", loc->accuracy);

            // Отображаем время последнего обновления
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
        

        // Перед созданием кнопки
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.8f, 1.0f));       
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));         

        if (ImGui::Button("Exit")) {
            running = false;
            loc->server_stop = true;
        }

        ImGui::PopStyleColor(2); 
        
        ImGui::End();







        
        // Рендеринг
        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        
        this_thread::sleep_for(std::chrono::milliseconds(16)); //ограничения FPS (частоты кадров).
    }
    
    // Очистка ресурсов
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    cout << "[GUI] GUI thread finished" << endl;
}