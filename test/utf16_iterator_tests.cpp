#include "gtest/gtest.h"
#include "utf16_testdata.h"
#include "utflib/iterators.h"
#include "utflib/low_level.h"
#include <span>
#include <cstdint>
#include <vector>
#include <optional>

//
// utf-16 iterator
//
TEST(utf16_iterator_empty_sequence, getters_and_go_methods) {
	std::span<std::uint16_t> td {};
	utf16_iterator it(td);
	EXPECT_TRUE(it.is_finished());
	EXPECT_TRUE(it.at_start());
	EXPECT_FALSE(it.get().has_value());
	EXPECT_FALSE(it.get_codepoint().has_value());
	EXPECT_FALSE(it.go_next());
	EXPECT_FALSE(it.go_prev());
}

TEST(utf16_iterator_to_utf32_forward, valid) {
	std::span<testdata_utf16_utf32_sequences> td = get_valid_utf16_sequences();
	for (const auto& e : td) {
		utf16_iterator it(e.utf16);
		std::size_t idx_u32 {0};
		while (!it.is_finished()) {
			ASSERT_TRUE(idx_u32 < e.utf32.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf16_codepoint> ou16 = it.get();
			ASSERT_TRUE(ocp.has_value());
			ASSERT_TRUE(ou16.has_value());
			EXPECT_EQ(ocp->get(), e.utf32[idx_u32]);
			// Verify that the iterator's utf16 and codepoint get()ters return the same thing
			codepoint cp(*ou16);
			EXPECT_EQ(*ocp, cp);
			it.go_next();
			++idx_u32;
		}
		EXPECT_TRUE(idx_u32 == e.utf32.size());
	}
}

TEST(utf16_iterator_to_utf32_backward, valid) {
	std::span<testdata_utf16_utf32_sequences> td = get_valid_utf16_sequences();
	for (const auto& e : td) {
		utf16_iterator it(e.utf16);
		while (!it.is_finished()) { it.go_next(); }
		std::size_t idx_u32 {e.utf32.size()};
		while (it.go_prev()) {
			--idx_u32;
			ASSERT_TRUE(idx_u32 < e.utf32.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf16_codepoint> ou16 = it.get();
			ASSERT_TRUE(ocp.has_value());
			ASSERT_TRUE(ou16.has_value());
			EXPECT_EQ(ocp->get(), e.utf32[idx_u32]);
			// Verify that the iterator's utf8 and codepoint get()ters return the same thing
			codepoint cp(*ou16);
			EXPECT_EQ(cp, *ocp);
		}
		EXPECT_TRUE(idx_u32 == 0);  // Verify the loop validated all codepoints
	}
}

TEST(utf16_iterator_to_utf32_forward, invalid) {
	std::span<testdata_utf16_utf32_sequences> td = get_invalid_utf16_sequences();
	for (const auto& e : td) {
		utf16_iterator it(e.utf16);
		std::size_t idx_u32 {0};
		while (!it.is_finished()) {
			ASSERT_TRUE(idx_u32 < e.utf32.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf16_codepoint> ou16 = it.get();
			if (ocp.has_value()) {
				ASSERT_TRUE(ou16.has_value());
				bool b = ocp->get() == e.utf32[idx_u32];
				EXPECT_EQ(ocp->get(), e.utf32[idx_u32]);
				// Verify that the iterator's utf16 and codepoint get()ters return the same thing
				codepoint cp(*ou16);
				EXPECT_EQ(*ocp, cp);
			} else {
				EXPECT_EQ(e.utf32[idx_u32], 0xFFFD);
				EXPECT_FALSE(ou16.has_value());
			}
			it.go_next();
			++idx_u32;
		}
		EXPECT_TRUE(idx_u32 == e.utf32.size());
	}
}

TEST(utf16_iterator_to_utf32_backward, invalid) {
	std::span<testdata_utf16_utf32_sequences> td = get_invalid_utf16_sequences();
	for (const auto& e : td) {
		utf16_iterator it(e.utf16);
		while (!it.is_finished()) { it.go_next(); }
		std::size_t idx_u32 {e.utf32.size()};
		while (it.go_prev()) {
			--idx_u32;
			ASSERT_TRUE(idx_u32 < e.utf32.size());
			std::optional<codepoint> ocp = it.get_codepoint();
			std::optional<utf16_codepoint> ou16 = it.get();
			if (ocp) {
				EXPECT_EQ(ocp->get(), e.utf32[idx_u32]);
				EXPECT_TRUE(ou16.has_value());
				// Verify that the iterator's utf16 and codepoint get()ters return the same thing
				codepoint cp(*ou16);
				EXPECT_EQ(cp, *ocp);
			} else {
				EXPECT_EQ(e.utf32[idx_u32], 0xFFFD);
				EXPECT_FALSE(ou16.has_value());
			}
		}
		EXPECT_TRUE(idx_u32 == 0);  // Verify the loop validated all codepoints
	}
}
