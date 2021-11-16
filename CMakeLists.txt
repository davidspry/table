cmake_minimum_required(VERSION 3.20)
project(Table)

set(CMAKE_CXX_STANDARD 17)

# Add tests
add_subdirectory(tests)

# Establish a header-only library
add_library(${PROJECT_NAME} INTERFACE)

# Include the header
target_include_directories(${PROJECT_NAME} INTERFACE include)