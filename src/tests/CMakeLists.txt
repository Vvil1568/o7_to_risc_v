cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
add_executable(o7p main.cpp)
include_directories(${CMAKE_SOURCE_DIR}/src/smodel)
target_link_libraries(o7p smodel)