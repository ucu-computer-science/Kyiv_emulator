cmake_minimum_required(VERSION 3.16)
project(kyivemu)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

include_directories(inc)

add_executable(kyivemu src/interface/mainwindow.cpp src/interface/mainwindow.ui
         inc/interface/mainwindow.h main.cpp src/emulator/asm_disasm.cpp src/emulator/mainKyiv.cpp
        inc/emulator/kyiv.h inc//emulator/asm_disasm.h)

target_link_libraries(kyivemu Boost::program_options)
target_link_libraries(kyivemu Qt5::Widgets)
