cmake_minimum_required(VERSION 3.5.1)
project(path_finding_visualizer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#include_directories(/usr/local/include)
include_directories(include)
include_directories(include/States)
include_directories(include/States/Algorithms/BFS)

set(EXECUTABLE_NAME "main")

add_library(main_menu
  src/States/MainMenu_State.cpp
)

add_library(bfs
  src/States/Algorithms/BFS/BFS.cpp
  src/States/Algorithms/BFS/Node.cpp
)

# Find any version 2.X of SFML
find_package(SFML 2.5.1 COMPONENTS system window graphics network audio)

if(SFML_FOUND)
    message(STATUS "SFML_VERSION: ${SFML_VERSION}")
endif()

add_executable(${EXECUTABLE_NAME}
  src/main.cpp
  src/Game.cpp
  src/State.cpp
  src/Gui.cpp
)

target_link_libraries(${EXECUTABLE_NAME}
  sfml-graphics
  sfml-window
  sfml-system
  main_menu
  bfs
)
