#pragma once
#include <vector>
#include <cstdint>
#include <span>
#include <array>

//
// UTF-8 Sequences
//

struct testdata_valid_utf8_utf32 {
	std::vector<std::uint8_t> utf8;
	std::vector<std::uint32_t> utf32;
};
std::span<testdata_valid_utf8_utf32> get_valid_utf8_utf32_sequences();

struct testdata_invalid_utf8_utf32 {
	std::vector<std::uint8_t> utf8;
	std::vector<std::uint32_t> utf32;
	std::vector<std::uint32_t> utf32_alt;  // Alternate substitution procedure
};
std::span<testdata_invalid_utf8_utf32> get_invalid_utf8_utf32_sequences();

//
// Single-byte tests
//
struct testdata_utf8_leading_bytes {
	std::uint8_t lb {};
	int sz_seq {};  // The number of bytes in the code unit sequence
};
std::span<testdata_utf8_leading_bytes> get_valid_utf8_leading_byte_testdata();
// Examples of invalid leading bytes
std::span<std::uint8_t> get_invalid_utf8_leading_byte_testdata();
// Examples of invalid third and fourth bytes
std::span<std::uint8_t> get_invalid_utf8_third_and_fourth_byte_testdata();
// Examples of valid third and fourth bytes
std::span<std::uint8_t> get_valid_utf8_third_and_fourth_byte_testdata();

struct testdata_utf8_first_and_second_bytes {
	std::uint8_t first {};
	std::uint8_t second {};
};
std::span<testdata_utf8_first_and_second_bytes> get_valid_first_and_second_utf8_byte_pairs();
std::span<testdata_utf8_first_and_second_bytes> get_invalid_first_and_second_utf8_byte_pairs();


//
// Single codepoint tests
//
struct testdata_valid_utf8 {
	std::array<std::uint8_t, 4> utf8 {};
	std::uint32_t cp {};
	int sz {};
};
std::span<testdata_valid_utf8> get_valid_utf8_single_cp_testdata();

struct testdata_invalid_utf8 {
	std::vector<std::uint8_t> utf8 {};
};
std::span<testdata_invalid_utf8> get_invalid_utf8_single_cp_testdata();


