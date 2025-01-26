#pragma once
#include "utflib/utflib.h"
#include "utflib/low_level.h"
#include <vector>
#include <filesystem>
#include <cstdint>
#include <random>
#include <span>

std::vector<std::byte> readfile(const std::filesystem::path& fp);

// Overwrites the file if it already exists
bool writefile(const std::filesystem::path& fp, std::span<const std::byte> data);



struct utf8_subseq_len_probability {
	double p_1 {1.0};
	double p_2 {0.0};
	double p_3 {0.0};
	double p_4 {0.0};
};
template<typename OIt>
OIt random_utf8_sequence(utf8_subseq_len_probability probs, int n_cp, std::default_random_engine& re, OIt out) {
	std::discrete_distribution<int> subseq_len_dist({probs.p_1, probs.p_2, probs.p_3, probs.p_4});

	for (int i=0; i<n_cp; ++i) {
		int n_bytes_curr = subseq_len_dist(re)+1;  // Note: +1
		std::uint32_t cp {0};
		if (n_bytes_curr == 1) {
			std::uniform_int_distribution<std::uint32_t> cp_val_dist(0x0u, 0x7Fu);
			cp = cp_val_dist(re);
		} else if (n_bytes_curr == 2) {
			std::uniform_int_distribution<std::uint32_t> cp_val_dist(0x80u, 0x7FFu);
			cp = cp_val_dist(re);
		} else if (n_bytes_curr == 3) {
			while (true) {
				std::uniform_int_distribution<std::uint32_t> cp_val_dist(0x800u, 0xFFFFu);
				cp = cp_val_dist(re);
				if (cp<=0xD7FFu || cp>=0xE000u) {
					break;
				}
			}
		} else if (n_bytes_curr == 4) {
			std::uniform_int_distribution<std::uint32_t> cp_val_dist(0x10000u, 0x10FFFFu);
			cp = cp_val_dist(re);
		} else {
			expect(false);
		}
		expect(is_valid_cp(cp));
		out = to_utf8(cp, out);
	}
	return out;
}


struct utf16_subseq_len_probability {
	double p_1 {1.0};
	double p_2 {0.0};
};
template<typename OIt>
OIt random_utf16_sequence(utf16_subseq_len_probability probs, int n_cp, std::default_random_engine& re, OIt out) {
	std::discrete_distribution<int> subseq_len_dist({probs.p_1, probs.p_2});

	for (int i=0; i<n_cp; ++i) {
		int n_words_curr = subseq_len_dist(re)+1;  // Note: +1
		std::uint32_t cp {0};
		if (n_words_curr == 1) {
			// [UTF-16 is] the Unicode encoding form that assigns each Unicode scalar value in the ranges
			// U+0000..U+D7FF and U+E000..U+FFFF to a single unsigned 16-bit code unit
			std::uniform_int_distribution<std::uint32_t> cp_val_dist(0x0u, 0xFFFFu);
			cp = cp_val_dist(re);
			while (cp>0xD7FFu && cp<0xE000) {
				cp = cp_val_dist(re);
			}
		} else if (n_words_curr == 2) {
			// ...and that assigns each Unicode scalar value in the range U+10000..U+10FFFF to a surrogate pair
			std::uniform_int_distribution<std::uint32_t> cp_val_dist(0x10000u, 0x10FFFFu);
			cp = cp_val_dist(re);
		} else {
			expect(false);
		}
		expect(is_valid_cp(cp));
		out = to_utf16(cp, out);
	}
	return out;
}

