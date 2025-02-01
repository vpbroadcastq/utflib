#include "utflib/low_level.h"

#include <cstdint>
#include <span>


void expect(bool b, const char* msg) {
	if (b) { return; }
	std::abort();
}

bool is_valid_cp(std::uint32_t cp) {
	return (cp <= 0xD7FFu)
		|| (cp>=0xE000u && cp<=0x10FFFFu);
}

// Valid leading bytes fall on [0x00, 0x7F] or [0xC2, 0xF4]
bool is_valid_utf8_leading_byte(std::uint8_t b) {
	return (b<=0x7F || (b>=0xC2 && b<=0xF4));
}

// Undefined if lb is not a valid leading byte.
bool is_valid_utf8_second_byte(std::uint8_t tb, std::uint8_t lb) {
	if (lb <= 0x7F) {
		return false;  // There is no second byte
	} else if (lb >= 0xC2 && lb <= 0xDF) {
		return tb >= 0x80 && tb <= 0xBF;
	} else if (lb == 0xE0) {
		return tb >= 0xA0 && tb <= 0xBF;
	} else if (lb >= 0xE1 && lb <= 0xEC) {
		return tb >= 0x80 && tb <= 0xBF;
	} else if (lb == 0xED) {
		return tb >= 0x80 && tb <= 0x9F;
	} else if (lb >= 0xEE && lb <= 0xEF) {
		return tb >= 0x80 && tb <= 0xBF;
	} else if (lb == 0xF0) {
		return tb >= 0x90 && tb <= 0xBF;
	} else if (lb >= 0xF1 && lb <= 0xF3) {
		return tb >= 0x80 && tb <= 0xBF;
	} else { // (lb == 0xF4)
		return tb >= 0x80 && tb <= 0x8F;
	}
}

bool is_valid_utf8_third_or_fourth_byte(std::uint8_t tb) {
	return tb >= 0x80 && tb <= 0xBF;
}

bool could_be_utf8_trailing_byte(std::uint8_t b) {
	return (b>=0x80 && b<=0xBF);
}

// From the first byte of a multibyte sequence, computes the number of bytes
// in the sequence.
// Result is undefined if b is not a valid leading byte of a multibyte sequence
int size_utf8_multibyte_seq_from_leading_byte(std::uint8_t b) {
	if ((b & 0b10000000) == 0) {
		return 1;
	} else if ((b & 0b11100000) == 0b11000000) {
		return 2;
	} else if ((b & 0b11110000) == 0b11100000) {
		return 3;
	} else {
		return 4;
	}
}

// The number of bytes required to encode the given codepoint as utf8.  The result is undefined if
// the given codepoint is not valid (!is_valid_cp(std::uint32_t)).
int size_utf8_multibyte_seq_from_codepoint(std::uint32_t cp) {
	if (cp <= 0x7Fu) { return 1; }
	if (cp <= 0x7FFu) { return 2; }
	if (cp <= 0xFFFFu) { return 3; }
	return 4;
}

// Extracts the value bits from the leading byte of a potentially multibyte sequence.
// sz_multib == size_utf8_multibyte_seq_from_codepoint_multibyte_seq_from_leading_byte(b); the result is undefined if sz_multib is incorrect
// or if b is not a valid leading byte (ie, if !is_valid_utf8_leading_byte(b)).
std::uint8_t payload_utf8_leading_byte(std::uint8_t b, int sz_multib) {
	if (sz_multib == 1) {
		return 0x7Fu & b;
	} else if (sz_multib == 2) {
		return 0x1Fu & b;
	} else if (sz_multib == 3) {
		return 0x0Fu & b;
	} else { // (sz_multib == 4) 
		return 0x07u & b;
	}
}

// Extracts the value bits from a trailing byte of a multibyte sequence.  The result
// is undefined if !is_valid_trailing_byte(b).
std::uint8_t payload_utf8_trailing_byte(std::uint8_t b) {
	return 0x3Fu & b;
}

bool begins_with_valid_utf8(std::span<const std::uint8_t> s) {
	if (s.size() == 0) {
		return false;
	}
	if (!is_valid_utf8_leading_byte(s[0])) {
		return false;
	}
	int sz = size_utf8_multibyte_seq_from_leading_byte(s[0]);
	if(s.size() < sz) {
		return false;
	}
	if (sz == 1) {
		return true;
	}
	if (!is_valid_utf8_second_byte(s[1],s[0])) {
		return false;
	}
	if (sz == 2) {
		return true;
	}
	if (!is_valid_utf8_third_or_fourth_byte(s[2])) {
		return false;
	}
	if (sz == 3) {
		return true;
	}
	if (!is_valid_utf8_third_or_fourth_byte(s[3])) {
		return false;
	}
	return true;
}

bool is_valid_utf8_single_codepoint(std::span<const std::uint8_t> s) {
	if (s.size() == 0) {
		return false;
	}
	if (!is_valid_utf8_leading_byte(s[0])) {
		return false;
	}
	int sz = size_utf8_multibyte_seq_from_leading_byte(s[0]);
	if (s.size() != sz) {
		return false;
	}
	if (sz == 1) {
		return true;
	}
	if (!is_valid_utf8_second_byte(s[1],s[0])) {
		return false;
	}
	if (sz == 2) {
		return true;
	}
	if (!is_valid_utf8_third_or_fourth_byte(s[2])) {
		return false;
	}
	if (sz == 3) {
		return true;
	}
	if (!is_valid_utf8_third_or_fourth_byte(s[3])) {
		return false;
	}
	return true;
}


// Begins at the start of the span and seeks to the first valid leading byte.
// p==end <=> sz==0
// There is no meaningful "error" state here that is different from "searched all the way to the end"
// Users should compare p to s.end(); if p<s.end(), sz~[1,4] && p+sz <= s.end()
// TODO:  Why not just return a span?
leading_byte_ptr_with_size seek_to_first_utf8_leading_byte(std::span<const std::uint8_t> s) {
	leading_byte_ptr_with_size result;
	result.p = s.data();
	const std::uint8_t* p_end = s.data() + s.size();
	while (true) {
		if (result.p==p_end) {
			result.sz = 0;
			break;
		}
		if (is_valid_utf8_leading_byte(*result.p)) {
			result.sz = size_utf8_multibyte_seq_from_leading_byte(*result.p);
			if (result.sz <= (p_end-result.p)) {
				break;  // success path
			}
		}
		++result.p;
	}
	return result;
}

// Gets the first valid sequence it finds starting at s.begin()
// Todo:  Computes, but then discards, the size?  Not really; the size is implicit in the span
std::span<const std::uint8_t> seek_to_first_valid_utf8_sequence(std::span<const std::uint8_t> s) {
	const std::uint8_t* p = s.data();
	const std::uint8_t* p_end = s.data() + s.size();

	while (true) {
		leading_byte_ptr_with_size start = seek_to_first_utf8_leading_byte({p,p_end});
		if (start.p == p_end) {
			break;  // No sequence found
		}
		p = start.p;
		++p;  // seek_to_first_utf8_leading_byte ensures that the span is long enough to do this
		int i=2;
		while (i<=start.sz) {
			if (i==2 && !is_valid_utf8_second_byte(*p, *start.p)) {
				break;
			} else if (i>2 && !is_valid_utf8_third_or_fourth_byte(*p)) {
				break;
			}
			++i;
			++p;  // seek_to_first_utf8_leading_byte ensures that the span is long enough
		}

		if (i > start.sz) {
			// Success path
			// If the loop above incremented i past start.sz it validated all the trailing bytes
			return {start.p,start.p+start.sz};
		}
	}
	// Error path
	return {p_end, p_end};
}


// Undefined if s is not a valid utf8 byte sequence
std::uint32_t to_utf32(std::span<const std::uint8_t> s) {
	const std::uint8_t* p = s.data();
	int sz = size_utf8_multibyte_seq_from_leading_byte(*p);
	std::uint32_t result {};
	result += payload_utf8_leading_byte(*p,sz);
	if (sz == 1) { return result; }
	result <<= 6;
	++p;
	result += payload_utf8_trailing_byte(*p);
	if (sz == 2) { return result; }
	result <<= 6;
	++p;
	result += payload_utf8_trailing_byte(*p);
	if (sz == 3) { return result; }
	result <<= 6;
	++p;
	result += payload_utf8_trailing_byte(*p);
	return result;
}


//
// UTF-16
//

bool is_valid_utf16_codepoint(std::uint16_t w) {
	// Could delegate to is_valid_cp(std::uint32_t cp), but in the worst case that has to make
	// three comparisons so this is possibly *slightly* more efficient, at least in the absence of
	// inlining and LTO that may eleminate the unnecessary comparison.
	return !(w>=0xD800u && w<=0xDFFFu);
}

// Returns true if the given word is valid as the leading word of a surrogate pair, false otherwise.
// is_valid_utf16_surrogate_pair_leading(w) => !is_valid_utf16_codepoint(w)
bool is_valid_utf16_surrogate_pair_leading(std::uint16_t w) {
	// This establishes that w falls on [0xD800, 0xDBFF], a subset of the range of invalid unicode
	// scalar values that can be represented by a single 16 bit integer [0xD800,0xDFFF].
	return ((w>>10) == 0b110110u);
}

// Returns true if the given word is valid as the trailing word of a surrogate pair, false otherwise.
bool is_valid_utf16_surrogate_pair_trailing(std::uint16_t w) {
	// This establishes that w falls on [0xDC00, 0xDFFF], a subset of the range of invalid unicode
	// scalar values that can be represented by a single 16 bit integer [0xD800,0xDFFF].
	return ((w>>10) == 0b110111u);
}

// TODO:  Could I do this more efficiently by converting to a codepoint?
bool is_valid_utf16_surrogate_pair(std::uint16_t lw, std::uint16_t tw) {
	return is_valid_utf16_surrogate_pair_leading(lw) && is_valid_utf16_surrogate_pair_trailing(tw);
}

std::uint16_t payload_leading_word_utf16_surrogate(std::uint16_t lw) {
	constexpr std::uint16_t xmask {0b0000'0000'0011'1111u};
	constexpr std::uint16_t wmask {0b0000'0011'1100'0000u};
	std::uint16_t wwww = ((wmask & lw)>>6);
	std::uint16_t xxxxxx = xmask & lw;
	std::uint16_t uuuuu = wwww + std::uint16_t {1};
	return ((uuuuu<<6) | xxxxxx);
}

std::uint16_t payload_trailing_word_utf16_surrogate(std::uint16_t tw) {
	return tw & 0b0000'0011'1111'1111u;
}

std::uint32_t utf16_to_codepoint_value(std::uint16_t w) {
	return w;
}

std::uint32_t utf16_to_codepoint_value(std::uint16_t lw, std::uint16_t tw) {
	std::uint32_t h = payload_leading_word_utf16_surrogate(lw)<<10;
	std::uint32_t l = payload_trailing_word_utf16_surrogate(tw);
	return h|l;
}

// Undefined if cp is ! a valid codepoint
int size_utf16_code_unit_seq_from_codepoint(std::uint32_t cp) {
	// UTF-16 encoding form: The Unicode encoding form that assigns each Unicode scalar value
	// in the ranges U+0000..U+D7FF and U+E000..U+FFFF to a single unsigned 16-bit code unit with
	// the same numeric value as the Unicode scalar value, and that assigns each Unicode scalar
	// value in the range U+10000..U+10FFFF to a surrogate pair, according to Table 3-5.
	if (cp>=0x10000u) {
		return 2;
	}
	return 1;
}


std::span<const std::uint16_t> seek_to_first_valid_utf16_sequence(std::span<const std::uint16_t> s) {
	const std::uint16_t* p = s.data();
	const std::uint16_t* const p_end = s.data() + s.size();
	// Find the first p >= s.data() for which
	// is_valid_utf16_codepoint(*p)
	// or
	// (p+1)>p_end && is_valid_utf16_surrogate_pair_leading(*p) && is_valid_utf16_surrogate_pair_trailing(*(p+1))
	// or
	// p == p_end;
	while (true) {
		if (p == p_end) {
			break;
		}

		if (is_valid_utf16_codepoint(*p)) {
			return {p, p+1};
		}
	
		if ((p_end-p)>1
			&& is_valid_utf16_surrogate_pair_leading(*p)
			&& is_valid_utf16_surrogate_pair_trailing(*(p+1))) {
			return {p, p+2};
		}

		++p;
	}

	return {p_end, p_end};  // Not reachable
}

// s.size()==1 && is_valid_utf16_codepoint(s[0])
// or
// s.size()==2 && is_valid_utf16_surrogate_pair(s[0],s[1])
bool is_valid_utf16_single_codepoint(std::span<const std::uint16_t> s) {
	if (s.size()==1 && is_valid_utf16_codepoint(s[0])) {
		return true;
	}
	if (s.size()==2 && is_valid_utf16_surrogate_pair(s[0],s[1])) {
		return true;
	}
	return false;
}
