cmake_minimum_required(VERSION 3.10)
project(path_finding_visualizer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

include_directories(/usr/local/include)
include_directories(include)
include_directories(include/States)
include_directories(include/States/Search_based)
include_directories(include/States/Sampling_based)
include_directories(include/States/Search_based/BFS)
include_directories(include/States/Search_based/DFS)
include_directories(include/States/Search_based/DIJKSTRA)
include_directories(include/States/Search_based/ASTAR)
include_directories(include/States/Sampling_based/RRT)
include_directories(include/States/Sampling_based/RRT_STAR)

set(EXECUTABLE_NAME "main")

add_executable(${EXECUTABLE_NAME}
  src/main.cpp
  src/Game.cpp
  src/State.cpp
  src/Gui.cpp
  src/States/Search_based/SearchBased.cpp
  src/States/Search_based/Node.cpp
  src/States/Sampling_based/SamplingBased.cpp
)

add_library(main_menu
  src/States/MainMenu_State.cpp
)

########################
# Search-based Planners
########################

add_library(bfs
  src/States/Search_based/BFS/BFS.cpp
)

add_library(dfs
  src/States/Search_based/DFS/DFS.cpp
)

add_library(dijkstra
  src/States/Search_based/DIJKSTRA/DIJKSTRA.cpp
)

add_library(astar
  src/States/Search_based/ASTAR/ASTAR.cpp
)

##########################
# Sampling-based Planners
##########################

add_library(rrt
  src/States/Sampling_based/RRT/RRT.cpp
)

add_library(rrt_star
  src/States/Sampling_based/RRT_STAR/RRT_STAR.cpp
)

# Find any version 2.X of SFML
find_package(SFML 2.5.1 COMPONENTS system window graphics network audio)
find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})

target_link_libraries(${EXECUTABLE_NAME}
  ${Boost_LIBRARIES}
  sfml-graphics
  sfml-window
  sfml-system
)

target_link_libraries(${EXECUTABLE_NAME}
  main_menu
  bfs
  dfs
  dijkstra
  astar
  rrt
	rrt_star
)
