#include "gtest/gtest.h"
#include "utf8_testdata.h"
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

// TODO:  Tests for utf16 encoder (in a different file)
