cmake_minimum_required(VERSION 3.16)
project(kyivemu)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

##################################
# Download and install GoogleTest

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.10.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

################


include_directories(inc)

add_executable(kyivemu src/main.cpp inc/kyiv.h inc/k_memory.h)

target_link_libraries(kyivemu Boost::program_options)

enable_testing()

add_subdirectory(test)
