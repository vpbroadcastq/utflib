#pragma once
#include <vector>
#include <cstdint>
#include <span>


//
// UTF-32 Sequences
//
std::span<std::vector<std::uint32_t>> get_valid_utf32_sequences();

struct testdata_invalid_utf32 {
	// Input sequence with invalid subsequences
	std::vector<std::uint32_t> utf32_invalid;
	// Valid sequences with invalid subsequences replaced with 0xFFFD
	std::vector<std::uint32_t> utf32;
	// Same as above but using the alternate substitution procedure
	std::vector<std::uint32_t> utf32_alt; 
};
std::span<testdata_invalid_utf32> get_invalid_utf32_sequences();

//
// Single-dword tests
//
std::span<std::uint32_t> get_valid_utf32_code_unit_testdata();
std::span<std::uint32_t> get_invalid_utf32_code_unit_testdata();


