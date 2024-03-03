cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
add_executable(o7p main.cpp)
target_link_libraries(o7p compiler)     # Линковка программы с библиотекой