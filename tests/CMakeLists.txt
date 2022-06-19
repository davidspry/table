cmake_minimum_required(VERSION 3.14)

include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
)

FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.6.1
)

FetchContent_MakeAvailable(googletest googlebenchmark)

enable_testing()
add_executable(TableTests test.cpp)
add_executable(TableBenchmark benchmark.cpp)

target_link_libraries(TableTests GTest::gtest)
target_link_libraries(TableBenchmark benchmark::benchmark)

include(GoogleTest)
gtest_discover_tests(TableTests)