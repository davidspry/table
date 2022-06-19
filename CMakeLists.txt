cmake_minimum_required(VERSION 3.14)
project(Table)

set(TARGET "table")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)

add_subdirectory(tests)
add_library(${TARGET} INTERFACE)
target_include_directories(${TARGET} INTERFACE include)