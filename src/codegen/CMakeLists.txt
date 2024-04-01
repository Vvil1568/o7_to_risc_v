# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Установка переменной со списком исходников
set(SOURCE_LIB 
    # headers
    registerpool.h
    codegencontext.h
    stackcontroller.h

    #sources
    registerpool.cpp
    codegencontext.cpp
    stackcontroller.cpp
)

add_library(codegen STATIC ${SOURCE_LIB} )   # Создание статической библиотеки
