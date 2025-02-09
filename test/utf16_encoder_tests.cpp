#include "gtest/gtest.h"
#include "encoder_testdata.h"
#include "utflib/encoders.h"
#include <span>
#include <cstdint>
#include <vector>
#include <optional>


TEST(encode_u32_to_u16, valid) {
	std::span<encoder_testdata> td = get_encoder_testdata_valid();
	for (const auto& curr_seq : td) {
		std::vector<std::uint16_t> v;
		auto it = std::back_inserter(v);
		for (std::uint32_t cp : curr_seq.u32) {
			it = to_utf16(cp, it);
		}
		EXPECT_EQ(v,curr_seq.u16);
	}
}
