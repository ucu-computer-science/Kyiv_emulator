cmake_minimum_required(VERSION 3.17)
project(KTests)

add_executable(KTests kyiv_test.cpp ../src/main.cpp)
target_link_libraries(KTests gtest_main)

add_test(
        NAME KTests
        COMMAND KTests )