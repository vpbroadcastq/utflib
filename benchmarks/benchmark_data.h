#pragma once
#include <span>
#include <cstdint>



std::span<const std::uint8_t> get_utf8_equal_probability_code_unit_seq_length_dataset_1();

//
// DATASET 2
//
// 500 lines with 20 codepoints per line; every 20 codepoints an 0x0A (\n) is inserted (hence "line"),
// for a total of 10,500 codepoints.
std::span<const std::uint32_t> get_random_codepoints_dataset_2_utf32();
std::span<const std::uint16_t> get_random_codepoints_dataset_2_utf16();
std::span<const std::uint8_t> get_random_codepoints_dataset_2_utf8();


