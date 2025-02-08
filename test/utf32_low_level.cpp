#include "gtest/gtest.h"
#include "utf32_testdata.h"
#include "utflib/low_level.h"
#include <span>
#include <cstdint>
#include <vector>



TEST(test_is_valid_utf32_codepoint, valid) {
	std::span<std::uint32_t> td = get_valid_utf32_code_unit_testdata();
	for (const auto& e : td) {
		EXPECT_TRUE(is_valid_utf32_codepoint(e));
	}
}

TEST(test_is_valid_utf32_codepoint, invalid) {
	std::span<std::uint32_t> td = get_invalid_utf32_code_unit_testdata();
	for (const auto& e : td) {
		EXPECT_FALSE(is_valid_utf32_codepoint(e));
	}
}




