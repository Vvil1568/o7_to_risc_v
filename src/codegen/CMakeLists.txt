# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# ��������� ���������� �� ������� ����������
set(SOURCE_LIB 
    # headers
    registerpool.h
    codegencontext.h

    #sources
    registerpool.cpp
    codegencontext.cpp
)

add_library(codegen STATIC ${SOURCE_LIB} )   # �������� ����������� ����������
