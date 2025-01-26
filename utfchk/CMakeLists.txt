cmake_minimum_required(VERSION 3.10)
project(utfchk VERSION 1.0 DESCRIPTION "Simple example application using utflib" LANGUAGES CXX)

# Add source to this project's executable.
add_executable(utfchk
	main.cpp
	utils.cpp
	test.cpp
  "wch2mb.cpp")

set_target_properties(utfchk PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(utfchk PRIVATE utflib)

