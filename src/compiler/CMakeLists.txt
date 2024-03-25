# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Установка переменной со списком исходников
set(SOURCE_LIB 
    # headers
    tlt.h
    #error.h
    parser.h
    
    #sources
    #error.cpp
    parser.cpp
)

add_library(compiler STATIC ${SOURCE_LIB})   # Создание статической библиотеки
include_directories(${CMAKE_SOURCE_DIR}/src/smodel)
include_directories(${CMAKE_SOURCE_DIR}/src/codegen)

target_link_libraries(compiler smodel codegen)     # Линковка программы с библиотекой
