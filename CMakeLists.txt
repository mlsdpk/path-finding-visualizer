cmake_minimum_required(VERSION 3.19)
project(path_finding_visualizer)

# set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

################
# dependencies #
################

include(cmake/CPM.cmake)

# SFML
CPMAddPackage("gh:SFML/SFML#3.0.0")

# ImGui
if (UNIX AND NOT APPLE)
  CPMAddPackage(
    NAME imgui
    VERSION 1.91.1
    GITHUB_REPOSITORY ocornut/imgui
    DOWNLOAD_ONLY TRUE)
  file(
    DOWNLOAD
    "https://gist.githubusercontent.com/rokups/f771217b2d530d170db5cb1e08e9a8f4/raw/4c2c14374ab878ca2f45daabfed4c156468e4e27/CMakeLists.txt"
    "${imgui_SOURCE_DIR}/CMakeLists.txt"
    EXPECTED_HASH
      SHA256=fd62f69364ce13a4f7633a9b50ae6672c466bcc44be60c69c45c0c6e225bb086)
  set(IMGUI_EXAMPLES FALSE)
  set(IMGUI_DEMO FALSE)
  set(IMGUI_ENABLE_STDLIB_SUPPORT TRUE)
  add_subdirectory(${imgui_SOURCE_DIR} EXCLUDE_FROM_ALL SYSTEM)
else()
  FetchContent_Declare(
      imgui
      GIT_REPOSITORY https://github.com/ocornut/imgui.git
      GIT_TAG v1.91.1
  )
  FetchContent_GetProperties(imgui)
  if (NOT imgui_POPULATED)
      FetchContent_Populate(imgui)
  endif ()
endif()

# ImGui-SFML
CPMAddPackage(
    GITHUB_REPOSITORY "eliasdaler/imgui-sfml"
    GIT_TAG "v3.0"
    OPTIONS 
      "IMGUI_SFML_FIND_SFML OFF"
      "IMGUI_DIR ${imgui_SOURCE_DIR}"
)
add_compile_definitions(IMGUI_DISABLE_DEMO_WINDOWS) # imgui-sfml doesn't compile demo by default

#########
# build #
#########

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
target_link_libraries(
  ${PROJECT_NAME} 
  PRIVATE 
    ImGui-SFML::ImGui-SFML)