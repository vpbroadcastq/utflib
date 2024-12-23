#include "gtest/gtest.h"
#include "test_data.h"
#include "utflib/utflib.h"

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(utfchk, valid_sequences) {
	for (const auto& e : g_td_valid) {
		const char* p = reinterpret_cast<const char*>(e.d.data());
		bool b = utfchk({p, e.d.size()});
		EXPECT_TRUE(b);
	}
}

TEST(utfchk, invalid_sequences) {
	for (const auto& e : g_td_invalid) {
		const char* p = reinterpret_cast<const char*>(e.d.data());
		bool b = utfchk({p, e.d.size()});
		EXPECT_FALSE(b);
	}
}
