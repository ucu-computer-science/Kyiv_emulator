cmake_minimum_required(VERSION 3.16)
project(kyivemu)

set(CMAKE_CXX_STANDARD 17)

add_executable(kyivemu main.cpp k_memory.h)
