# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

find_package(Boost 1.72 REQUIRED program_options)
include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-DBOOST_ALL_NO_LIB)

set(BUILD_DIR "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/../bin")

add_executable(o7p main.cpp)

target_link_libraries(o7p ${Boost_LIBRARIES})
target_compile_features(o7p PRIVATE cxx_auto_type cxx_range_for)


target_link_libraries(o7p compiler)     # Линковка программы с библиотекой
