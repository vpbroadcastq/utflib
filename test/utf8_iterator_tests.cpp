#include "gtest/gtest.h"
#include "utf8_testdata.h"
#include "utflib/iterators.h"
#include <span>
#include <cstdint>
#include <vector>
#include <optional>


//
// iterator
//
TEST(iterator_to_utf32_forward, valid) {
	std::span<testdata_valid_utf8_utf32> td = get_valid_utf8_utf32_sequences();
	for (const auto& e : td) {
		utf8_iterator it(e.utf8);
		std::size_t idx_u32 {0};
		while (!it.is_finished()) {
			ASSERT_TRUE(idx_u32 < e.utf32.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf8_codepoint> ou8 = it.get_utf8();
			ASSERT_TRUE(ocp.has_value());
			ASSERT_TRUE(ou8.has_value());
			EXPECT_EQ(ocp->get(), e.utf32[idx_u32]);
			// Verify that the iterator's utf8 and codepoint getters return the same thing
			codepoint cp(*ou8);
			EXPECT_EQ(*ocp, cp);
			it.go_next();
			++idx_u32;
		}
		EXPECT_TRUE(idx_u32 == e.utf32.size());
	}
}

TEST(iterator_to_utf32_backward, valid) {
	std::span<testdata_valid_utf8_utf32> td = get_valid_utf8_utf32_sequences();
	for (const auto& e : td) {
		utf8_iterator it(e.utf8);
		while (!it.is_finished()) { it.go_next(); }
		std::size_t idx_u32 {e.utf32.size()};
		while (it.go_prev()) {
			--idx_u32;
			ASSERT_TRUE(idx_u32 < e.utf32.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf8_codepoint> ou8 = it.get_utf8();
			ASSERT_TRUE(ocp.has_value());
			ASSERT_TRUE(ou8.has_value());
			EXPECT_EQ(ocp->get(), e.utf32[idx_u32]);
			// Verify that the iterator's utf8 and codepoint getters return the same thing
			codepoint cp(*ou8);
			EXPECT_EQ(cp, *ocp);
		}
		EXPECT_TRUE(idx_u32 == 0);  // Verify the loop validated all codepoints
	}
}

TEST(iterator_to_utf32_forward, invalid) {
	std::span<testdata_invalid_utf8_utf32> td = get_invalid_utf8_utf32_sequences();
	int entry_num {0};
	for (const auto& e : td) {
		utf8_iterator it(e.utf8);
		std::size_t idx_u32 {0};
		while (!it.is_finished()) {
			ASSERT_TRUE(idx_u32 < e.utf32.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf8_codepoint> ou8 = it.get_utf8();
			if (ocp) {
				EXPECT_EQ(ocp->get(), e.utf32[idx_u32]);
				EXPECT_TRUE(ou8.has_value());
				// Verify that the iterator's utf8 and codepoint getters return the same thing
				codepoint cp(*ou8);
				EXPECT_EQ(cp, *ocp);
			} else {
				EXPECT_EQ(e.utf32[idx_u32], 0xFFFD);
				EXPECT_FALSE(ou8.has_value());
			}
			it.go_next();
			++idx_u32;
		}
		EXPECT_TRUE(idx_u32 == e.utf32.size());
		++entry_num;
	}
}

TEST(iterator_to_utf32_backward, invalid) {
	std::span<testdata_invalid_utf8_utf32> td = get_invalid_utf8_utf32_sequences();
	int dataset_num {0};
	for (const auto& e : td) {
		utf8_iterator it(e.utf8);
		while (!it.is_finished()) { it.go_next(); }
		std::size_t idx_u32 {e.utf32.size()};
		while (it.go_prev()) {
			--idx_u32;
			ASSERT_TRUE(idx_u32 < e.utf32.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf8_codepoint> ou8 = it.get_utf8();
			if (ocp) {
				EXPECT_EQ(ocp->get(), e.utf32[idx_u32]);
				EXPECT_TRUE(ou8.has_value());
				// Verify that the iterator's utf8 and codepoint getters return the same thing
				codepoint cp(*ou8);
				EXPECT_EQ(cp, *ocp);
			} else {
				EXPECT_EQ(e.utf32[idx_u32], 0xFFFD);
				EXPECT_FALSE(ou8.has_value());
			}
		}
		EXPECT_TRUE(idx_u32 == 0);  // Verify the loop validated all codepoints
		++dataset_num;
	}
}

