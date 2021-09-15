cmake_minimum_required(VERSION 3.20)
project(table)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)
add_executable(table main.cpp)
target_link_libraries(table GTest::gtest)