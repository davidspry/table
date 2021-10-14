find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

add_executable(TableTest test.cpp)
add_executable(TableBenchmark benchmark.cpp)

target_link_libraries(TableTest GTest::gtest)
target_link_libraries(TableBenchmark benchmark::benchmark)