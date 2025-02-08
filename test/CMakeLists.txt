cmake_minimum_required(VERSION 3.10)
project(test VERSION 1.0 DESCRIPTION "Unit tests for utflib" LANGUAGES CXX)

# Add source to this project's executable.
add_executable(test
	main.cpp
 "utf8_testdata.cpp" "utf32_testdata.h" "utf8_iterator_tests.cpp" "utf8_iterator_alt_tests.cpp" "utf8_low_level.cpp" "utf8_encoder_tests.cpp" "utf16_testdata.cpp" "utf16_testdata.h" "utf16_low_level.cpp" "utf16_iterator_tests.cpp"   "utf16_iterator_alt_tests.cpp" "utf8_testdata.h" "utf32_testdata.cpp" "utf32_low_level.cpp" "utf32_iterator_tests.cpp" "utf32_iterator_alt_tests.cpp" "encoder_testdata.h" "encoder_testdata.cpp" "utf16_encoder_tests.cpp")

set_target_properties(test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(test PRIVATE utflib)
target_link_libraries(test PRIVATE GTest::gtest)
