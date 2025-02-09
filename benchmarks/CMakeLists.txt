cmake_minimum_required(VERSION 3.10)
project(test VERSION 1.0 DESCRIPTION "Benchmarks for utflib" LANGUAGES CXX)

# Add source to this project's executable.
add_executable(benchmarks
	main.cpp "u8_iterators.cpp" "benchmark_data.h" "behcnmark_data.cpp" "u8_encoding.cpp")

set_target_properties(benchmarks PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(benchmarks PRIVATE utflib)
target_link_libraries(benchmarks PRIVATE benchmark::benchmark)
