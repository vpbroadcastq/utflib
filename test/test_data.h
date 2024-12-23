#pragma once
#include <vector>
#include <cstdint>


//
// Sequences
//
struct test_data {
	std::vector<std::uint8_t> d;
	std::vector<std::uint32_t> codepoints;
	std::size_t idx_init_invalid;
	bool is_valid;
};

extern std::vector<test_data> g_td_valid;
extern std::vector<test_data> g_td_invalid;

//
// Single-byte tests
//
extern std::array<std::uint8_t> valid_leading_bytes;
extern std::array<std::uint8_t> invalid_leading_bytes;

extern std::array<std::uint8_t> valid_trailing_bytes;
extern std::array<std::uint8_t> invalid_trailing_bytes;
