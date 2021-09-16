cmake_minimum_required(VERSION 3.20)
project(table)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)
add_executable(test test.cpp)
target_link_libraries(test GTest::gtest)

find_package(benchmark REQUIRED)
add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark benchmark::benchmark)