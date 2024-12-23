cmake_minimum_required(VERSION 3.10)
project(test VERSION 1.0 DESCRIPTION "Unit tests for utflib" LANGUAGES CXX)

# Add source to this project's executable.
add_executable(test
	main.cpp
 "test_data.cpp" "test_data.h")

set_target_properties(test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(test PRIVATE utflib)
target_link_libraries(test PRIVATE GTest::gtest)
