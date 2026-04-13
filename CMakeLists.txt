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
target_sources(pot PRIVATE FILE_SET stb TYPE HEADERS BASE_DIRS ${Stb_INCLUDE_DIR} FILES ${Stb_INCLUDE_DIR}/stb_image.h)

add_custom_command(TARGET pot
  POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory_if_newer
  ${PROJECT_SOURCE_DIR}/resources "$<PATH:APPEND,$<TARGET_FILE_DIR:pot>,resources>")

install(TARGETS pot)
install(DIRECTORY resources DESTINATION TYPE BIN)
