cmake_minimum_required(VERSION 3.15)
project(path_finding_visualizer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories(/usr/local/include)
include_directories(include)
include_directories(include/States)
include_directories(include/States/Algorithms/BFS)
include_directories(include/Elements)

set(EXECUTABLE_NAME "main")

add_executable(${EXECUTABLE_NAME}
  src/main.cpp
  src/Game.cpp
  src/State.cpp
)

add_library(main_menu
  src/States/MainMenu_State.cpp
)

add_library(bfs
  src/States/Algorithms/BFS/BFS.cpp
  src/States/Algorithms/BFS/Node.cpp
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
)
