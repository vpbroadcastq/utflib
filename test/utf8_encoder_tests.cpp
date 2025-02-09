#include "gtest/gtest.h"
#include "utf8_testdata.h"
#include "encoder_testdata.h"
#include "utflib/encoders.h"
#include <span>
#include <cstdint>
#include <vector>
#include <optional>


TEST(test_to_utf8, valid) {
	std::span<testdata_valid_utf8> td = get_valid_utf8_single_cp_testdata();
	for (const auto& e : td) {
		std::vector<std::uint8_t> v;
		auto it = to_utf8(e.cp, std::back_inserter(v));
		EXPECT_EQ(v.size(),e.sz);
		for (int i=0; i<v.size(); ++i) {
			EXPECT_EQ(v[i],e.utf8[i]);
		}
	}
}

TEST(encode_u32_to_u8, valid) {
	std::span<encoder_testdata> td = get_encoder_testdata_valid();
	for (const auto& curr_seq : td) {
		std::vector<std::uint8_t> v;
		auto it = std::back_inserter(v);
		for (std::uint32_t cp : curr_seq.u32) {
			it = to_utf8(cp, it);
		}
		EXPECT_EQ(v,curr_seq.u8);
	}
}

// TODO:  Tests for utf16 encoder (in a different file)
