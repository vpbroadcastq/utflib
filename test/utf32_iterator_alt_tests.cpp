#include "gtest/gtest.h"
#include "utf32_testdata.h"
#include "utflib/iterators.h"
#include <span>
#include <cstdint>
#include <vector>
#include <optional>


//
// utf-32 iterator
//
TEST(utf32_iterator_alt_forward, valid) {
	std::span<std::vector<std::uint32_t>> td = get_valid_utf32_sequences();
	for (const auto& e : td) {
		utf32_iterator_alt it(e);
		std::size_t idx {0};
		while (!it.is_finished()) {
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf32_codepoint> ou32 = it.get_utf32();
			ASSERT_TRUE(ocp.has_value());
			ASSERT_TRUE(ou32.has_value());
			EXPECT_EQ(ocp->get(), e[idx]);
			// Verify that the iterator's utf32 and codepoint getters return the same thing
			codepoint cp(*ou32);
			EXPECT_EQ(*ocp, cp);
			it.go_next();
			++idx;
		}
		EXPECT_TRUE(idx == e.size());
	}
}

TEST(utf32_iterator_alt_backward, valid) {
	std::span<std::vector<std::uint32_t>> td = get_valid_utf32_sequences();
	for (const auto& e : td) {
		utf32_iterator_alt it(e);
		while (!it.is_finished()) { it.go_next(); }
		std::size_t idx {e.size()};
		while (it.go_prev()) {
			--idx;
			ASSERT_TRUE(idx < e.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf32_codepoint> ou32 = it.get_utf32();
			ASSERT_TRUE(ocp.has_value());
			ASSERT_TRUE(ou32.has_value());
			EXPECT_EQ(ocp->get(), e[idx]);
			// Verify that the iterator's utf32 and codepoint getters return the same thing
			codepoint cp(*ou32);
			EXPECT_EQ(cp, *ocp);
		}
		EXPECT_TRUE(idx == 0);  // Verify the loop validated all codepoints
	}
}


TEST(utf32_iterator_alt_forward, invalid) {
	std::span<testdata_invalid_utf32> td = get_invalid_utf32_sequences();
	int entry_num {0};
	for (const auto& e : td) {
		utf32_iterator_alt it(e.utf32_invalid);
		std::size_t idx_u32alt {0};
		while (!it.is_finished()) {
			ASSERT_TRUE(idx_u32alt < e.utf32_alt.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf32_codepoint> ou32 = it.get_utf32();
			if (ocp) {
				EXPECT_EQ(ocp->get(), e.utf32_alt[idx_u32alt]);
				EXPECT_TRUE(ou32.has_value());
				// Verify that the iterator's utf32 and codepoint getters return the same thing
				codepoint cp(*ou32);
				EXPECT_EQ(cp, *ocp);
			} else {
				EXPECT_EQ(e.utf32_alt[idx_u32alt], 0xFFFDu);
				EXPECT_FALSE(ou32.has_value());
			}
			it.go_next();
			++idx_u32alt;
		}
		bool b = idx_u32alt == e.utf32_alt.size();
		EXPECT_TRUE(idx_u32alt == e.utf32_alt.size());
		++entry_num;
	}
}

TEST(utf32_iterator_alt_backward, invalid) {
	std::span<testdata_invalid_utf32> td = get_invalid_utf32_sequences();
	int dataset_num {0};
	for (const auto& e : td) {
		utf32_iterator_alt it(e.utf32_invalid);
		while (!it.is_finished()) { it.go_next(); }
		std::size_t idx_u32alt {e.utf32_alt.size()};
		while (it.go_prev()) {
			--idx_u32alt;
			ASSERT_TRUE(idx_u32alt < e.utf32_alt.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf32_codepoint> ou32 = it.get_utf32();
			if (ocp) {
				bool b = ocp->get()==e.utf32_alt[idx_u32alt];
				EXPECT_EQ(ocp->get(), e.utf32_alt[idx_u32alt]);
				EXPECT_TRUE(ou32.has_value());
				// Verify that the iterator's utf8 and codepoint getters return the same thing
				codepoint cp(*ou32);
				EXPECT_EQ(cp, *ocp);
			} else {
				EXPECT_EQ(e.utf32_alt[idx_u32alt], 0xFFFDu);
				EXPECT_FALSE(ou32.has_value());
			}
		}
		EXPECT_TRUE(idx_u32alt == 0);  // Verify the loop validated all codepoints
		++dataset_num;
	}
}

