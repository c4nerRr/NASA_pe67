cmake_minimum_required(VERSION 4.0)
project(NASA)

set(CMAKE_CXX_STANDARD 20)

add_executable(NASA main.cpp
        Init.cpp
        Init.h)
