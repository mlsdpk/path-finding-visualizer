cmake_minimum_required(VERSION 3.10)
project(path_finding_visualizer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

include_directories(/usr/local/include)
include_directories(include)
include_directories(include/States)
include_directories(include/States/Algorithms)
include_directories(include/States/Algorithms/BFS)
include_directories(include/States/Algorithms/DFS)

set(EXECUTABLE_NAME "main")

add_executable(${EXECUTABLE_NAME}
  src/main.cpp
  src/Game.cpp
  src/State.cpp
  src/Gui.cpp
  src/States/Algorithms/Algorithm.cpp
  src/States/Algorithms/Node.cpp
)

add_library(main_menu
  src/States/MainMenu_State.cpp
)

add_library(bfs
  src/States/Algorithms/BFS/BFS.cpp
)

add_library(dfs
  src/States/Algorithms/DFS/DFS.cpp
)

# Find any version 2.X of SFML
find_package(SFML 2.5.1 COMPONENTS system window graphics network audio)

include_directories(${SFML_INCLUDE_DIRS})

target_link_libraries(${EXECUTABLE_NAME}
  sfml-graphics
  sfml-window
  sfml-system
  main_menu
  bfs
  dfs
)
