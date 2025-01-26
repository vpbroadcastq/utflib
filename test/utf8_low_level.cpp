#include "gtest/gtest.h"
#include "utf8_testdata.h"
#include "utflib/low_level.h"
#include <span>
#include <cstdint>
#include <vector>
#include <optional>


TEST(test_is_valid_utf8_single_codepoint, valid) {
	std::span<testdata_valid_utf8> td = get_valid_utf8_single_cp_testdata();
	for (const auto& e : td) {
		bool b = is_valid_utf8_single_codepoint(e.utf8);
		EXPECT_TRUE(is_valid_utf8_single_codepoint({e.utf8.data(), e.utf8.data() + e.sz}));
	}
}

TEST(test_is_valid_utf8_single_codepoint, invalid) {
	std::span<testdata_invalid_utf8> td = get_invalid_utf8_single_cp_testdata();
	for (const auto& e : td) {
		bool b = is_valid_utf8_single_codepoint(e.utf8);
		EXPECT_FALSE(is_valid_utf8_single_codepoint(e.utf8));
	}
}

TEST(test_is_valid_cp, valid) {
	std::span<testdata_valid_utf8> td = get_valid_utf8_single_cp_testdata();
	for (const auto& e : td) {
		EXPECT_TRUE(is_valid_cp(e.cp));
	}
}

TEST(test_size_utf8_multibyte_seq_from_leading_byte, valid) {
	std::span<testdata_valid_utf8> td = get_valid_utf8_single_cp_testdata();
	for (const auto& e : td) {
		ASSERT_TRUE(e.utf8.size()>0);
		EXPECT_EQ(size_utf8_multibyte_seq_from_leading_byte(e.utf8[0]),e.sz);
	}
}

TEST(test_to_utf32, valid) {
	std::span<testdata_valid_utf8> td = get_valid_utf8_single_cp_testdata();
	for (const auto& e : td) {
		std::uint32_t u32 = to_utf32(e.utf8);
		EXPECT_EQ(u32,e.cp);
	}
}

TEST(test_leading_bytes, valid) {
	std::span<testdata_utf8_leading_bytes> td = get_valid_utf8_leading_byte_testdata();
	for (const auto& e : td) {
		EXPECT_TRUE(is_valid_utf8_leading_byte(e.lb));
	}
}

TEST(test_size_utf8_multibyte_seq_from_leading_byte, valid_leading_byte_testset) {
	std::span<testdata_utf8_leading_bytes> td = get_valid_utf8_leading_byte_testdata();
	for (const auto& e : td) {
		EXPECT_EQ(size_utf8_multibyte_seq_from_leading_byte(e.lb),e.sz_seq);
	}
}

TEST(test_leading_bytes, invalid) {
	std::span<std::uint8_t> td = get_invalid_utf8_leading_byte_testdata();
	for (const auto& e : td) {
		EXPECT_FALSE(is_valid_utf8_leading_byte(e));
	}
}

TEST(test_third_and_fourth_bytes, valid) {
	std::span<std::uint8_t> td = get_valid_utf8_third_and_fourth_byte_testdata();
	for (const auto& e : td) {
		EXPECT_TRUE(is_valid_utf8_third_or_fourth_byte(e));
	}
}

TEST(test_third_and_fourth_bytes, invalid) {
	std::span<std::uint8_t> td = get_invalid_utf8_third_and_fourth_byte_testdata();
	for (const auto& e : td) {
		EXPECT_FALSE(is_valid_utf8_third_or_fourth_byte(e));
	}
}

TEST(test_second_byte, valid) {
	std::span<testdata_utf8_first_and_second_bytes> td = get_valid_first_and_second_utf8_byte_pairs();
	for (const auto& e : td) {
		EXPECT_TRUE(is_valid_utf8_leading_byte(e.first));
		EXPECT_TRUE(is_valid_utf8_second_byte(e.second,e.first));
	}
}

TEST(test_second_byte, invalid) {
	std::span<testdata_utf8_first_and_second_bytes> td = get_invalid_first_and_second_utf8_byte_pairs();
	for (const auto& e : td) {
		EXPECT_TRUE(is_valid_utf8_leading_byte(e.first));
		EXPECT_FALSE(is_valid_utf8_second_byte(e.second,e.first));
	}
}




