cmake_minimum_required(VERSION 3.16)
project(kyivemu)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include_directories(inc)

add_executable(kyivemu src/main.cpp inc/kyiv.h)

target_link_libraries(kyivemu Boost::program_options)
