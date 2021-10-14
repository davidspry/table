cmake_minimum_required(VERSION 3.20)
project(Table)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(tests)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE include)