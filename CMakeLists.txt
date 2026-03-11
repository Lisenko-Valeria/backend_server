cmake_minimum_required(VERSION 3.14)
project(lab9)

set(CMAKE_CXX_STANDARD 17)

# Пути
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Поиск библиотек
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# ZeroMQ через pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq>=4.3.4)

# Заголовочные файлы
include_directories(${INCLUDE_DIR} 
                    ${SDL2_INCLUDE_DIRS} 
                    ${GLEW_INCLUDE_DIRS}
                    ${EXTERNAL_DIR}/imgui 
                    ${EXTERNAL_DIR}/implot 
                    ${EXTERNAL_DIR}/json/include)  

# Исходники ImGui
file(GLOB IMGUI_SOURCES ${EXTERNAL_DIR}/imgui/*.cpp
                        ${EXTERNAL_DIR}/imgui/backends/imgui_impl_sdl2.cpp
                        ${EXTERNAL_DIR}/imgui/backends/imgui_impl_opengl3.cpp)

file(GLOB IMPLOT_SOURCES ${EXTERNAL_DIR}/implot/implot.cpp
                         ${EXTERNAL_DIR}/implot/implot_items.cpp)

# Исполняемый файл 
add_executable(lab9 ${SRC_DIR}/main.cpp
                    ${SRC_DIR}/gui_app.cpp
                    ${SRC_DIR}/zmq_server.cpp  
                    ${IMGUI_SOURCES}
                    ${IMPLOT_SOURCES})

# Подключение библиотек
target_link_libraries(lab9 ${SDL2_LIBRARIES}
                           ${OPENGL_LIBRARIES}
                           ${GLEW_LIBRARIES}
                           ${ZMQ_LIBRARIES}
                           pthread dl)