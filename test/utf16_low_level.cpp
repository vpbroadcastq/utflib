#include "gtest/gtest.h"
#include "utf16_testdata.h"
#include "utflib/low_level.h"
#include <span>
#include <cstdint>
#include <vector>
#include <optional>


TEST(test_is_valid_utf16_single_word_codepoint, valid) {
	std::span<utf16_single_word_testdata> td = get_valid_utf16_single_word_data();
	for (const auto& e : td) {
		EXPECT_TRUE(is_valid_utf16_codepoint(e.w));
	}
}

TEST(test_is_valid_utf16_single_word_codepoint, invalid) {
	std::span<utf16_single_word_testdata> td = get_invalid_utf16_single_word_data();
	for (const auto& e : td) {
		EXPECT_FALSE(is_valid_utf16_codepoint(e.w));
	}
}

TEST(test_is_valid_utf16_surrogate_pair_leading, valid_and_invalid) {
	std::span<utf16_single_surrogate_testdata> td = get_utf16_leading_surrogate_test_data();
	for (const auto& e : td) {
		if (e.payload) {
			EXPECT_TRUE(is_valid_utf16_surrogate_pair_leading(e.w));
		} else {
			EXPECT_FALSE(is_valid_utf16_surrogate_pair_leading(e.w));
		}
	}
}

TEST(test_is_valid_utf16_surrogate_pair_trailing, valid_and_invalid) {
	std::span<utf16_single_surrogate_testdata> td = get_utf16_trailing_surrogate_test_data();
	for (const auto& e : td) {
		if (e.payload) {
			EXPECT_TRUE(is_valid_utf16_surrogate_pair_trailing(e.w));
		} else {
			EXPECT_FALSE(is_valid_utf16_surrogate_pair_trailing(e.w));
		}
	}
}

TEST(test_payload_leading_word_utf16_surrogate, valid) {
	std::span<utf16_single_surrogate_testdata> td = get_utf16_leading_surrogate_test_data();
	for (const auto& e : td) {
		if (!e.payload) { continue; }
		EXPECT_EQ(payload_leading_word_utf16_surrogate(e.w),*(e.payload));
	}
}

TEST(test_payload_trailing_word_utf16_surrogate, valid) {
	std::span<utf16_single_surrogate_testdata> td = get_utf16_trailing_surrogate_test_data();
	for (const auto& e : td) {
		if (!e.payload) { continue; }
		EXPECT_EQ(payload_trailing_word_utf16_surrogate(e.w),*(e.payload));
	}
}

TEST(test_utf16_to_codepoint_value, single_word) {
	std::span<utf16_single_word_testdata> td = get_valid_utf16_single_word_data();
	for (const auto& e : td) {
		ASSERT_TRUE(e.codepoint.has_value());
		EXPECT_EQ(utf16_to_codepoint_value(e.w), *(e.codepoint));
		// Bonus:  Since these are the single code-unit testdata...
		EXPECT_FALSE(is_valid_utf16_surrogate_pair_leading(e.w));
		EXPECT_FALSE(is_valid_utf16_surrogate_pair_trailing(e.w));
	}
}

TEST(test_utf16_to_codepoint_value, surrogate_pairs) {
	std::span<utf16_valid_surrogate_pair_testdata> td = get_valid_utf16_surrogate_pair_test_data();
	for (const auto& e : td) {
		// Since this is the "valid" set of testdata...
		EXPECT_TRUE(is_valid_utf16_surrogate_pair_leading(e.lw));
		EXPECT_TRUE(is_valid_utf16_surrogate_pair_trailing(e.tw));
		EXPECT_EQ(utf16_to_codepoint_value(e.lw, e.tw), e.cp);
	}
}


TEST(test_size_utf16_code_unit_seq_from_codepoint, valid_codepoints) {
	std::span<utf16_codepoint_and_code_unit_seq_len_testdata> td = get_utf16_valid_codepoint_to_codeunit_seq_len_testdata();
	for (const auto& e : td) {
		int n = size_utf16_code_unit_seq_from_codepoint(e.codepoint);
		ASSERT_TRUE(e.size.has_value());
		EXPECT_EQ(e.size,n);
	}
}


