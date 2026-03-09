cmake_minimum_required(VERSION 4.0)

project(pixart CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(GLEW REQUIRED)

add_executable(pot)
target_sources(pot PRIVATE main.cpp)
target_compile_features(pot PRIVATE cxx_std_20)
target_link_libraries(pot PRIVATE OpenGL::GL glfw imgui::imgui GLEW::GLEW)
target_include_directories(pot PRIVATE ${Stb_INCLUDE_DIR})
