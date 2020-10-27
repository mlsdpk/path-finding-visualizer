cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(path_finding_visualizer)
add_definitions(-std=c++11)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(EXECUTABLE_NAME "main")
add_executable(${EXECUTABLE_NAME} src/main.cpp)

link_directories(SFML_LIBRARY_DIR)
include_directories(SFML_INCLUDE_DIR)

# Find any version 2.X of SFML
find_package(SFML 2.5.1 COMPONENTS system window graphics network audio)

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} sfml-graphics sfml-window sfml-system)
