cmake_minimum_required(VERSION 3.26)
project(GalaxyExplorer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(SOURCE_FILES main.cpp)
add_executable(GalaxyExplorer ${SOURCE_FILES})
include_directories(/usr/local/include)

find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(GalaxyExplorer sfml-system sfml-window sfml-graphics sfml-audio sfml-network)