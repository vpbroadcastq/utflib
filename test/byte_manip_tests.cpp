#include "gtest/gtest.h"
#include "utflib/byte_manip.h"
#include <span>
#include <cstdint>
#include <vector>
#include <optional>


TEST(reverse_bytes, uint32) {
	std::vector<std::pair<std::uint32_t, std::uint32_t>> td {{
		{0xAA'BB'CC'DDu, 0xDD'CC'BB'AAu},
		{0x10'00'00'00u, 0x00'00'00'10u},
		{0xFF'FF'FF'FFu, 0xFF'FF'FF'FFu},
	}};
	for (const auto& e : td) {
		EXPECT_EQ(reverse_bytes(e.first), e.second);
	}
}
