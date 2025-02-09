cmake_minimum_required(VERSION 3.10)
project(utflib VERSION 1.0 DESCRIPTION "UTF processing library" LANGUAGES NONE)

# Create library from SOURCE_FILES
add_library(utflib STATIC "src/utflib.cpp" "include/utflib/low_level.h" "include/utflib/utflib.h" "src/low_level.cpp" "include/utflib/iterators.h" "src/iterators.cpp" "include/utflib/encoders.h" "src/encoders.cpp")

set_target_properties(utflib PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# From libmemops:
#target_include_directories(utflib INTERFACE
#	${CMAKE_CURRENT_SOURCE_DIR}/include
#)

target_include_directories(utflib PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# From libmemops:
# Set the output directory for the library files
#set_target_properties(utflib PROPERTIES
#    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#)
