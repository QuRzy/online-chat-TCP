cmake_minimum_required(VERSION 3.17)
project(serverChat)

set(CMAKE_CXX_STANDARD 20)
find_package(SFML COMPONENTS network  REQUIRED)
add_executable(serverChat main.cpp)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(serverChat sfml-network)