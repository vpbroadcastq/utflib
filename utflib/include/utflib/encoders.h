#pragma once
#include <cstdint>
#include "low_level.h"

// TODO:  decoders.h/.cpp for reading in files; handle byte-order issues

// Undefined if cp is not a valid codepoint
template<typename OIt>
OIt to_utf8(std::uint32_t cp, OIt out) {
	// Table 3-6. UTF-8 Bit Distribution
	// Scalar Value                 First Byte    Second Byte    Third Byte    Fourth Byte
	// 00000000 0xxxxxxx            0xxxxxxx
	// 00000yyy yyxxxxxx            110yyyyy      10xxxxxx
	// zzzzyyyy yyxxxxxx            1110zzzz      10yyyyyy       10xxxxxx
	// 000uuuuu zzzzyyyy yyxxxxxx   11110uuu      10uuzzzz       10yyyyyy      10xxxxxx
	int sz = size_utf8_multibyte_seq_from_codepoint(cp);
	if (sz == 1) {
		*out = static_cast<std::uint8_t>(cp);
		++out;
	} else if (sz == 2) {
		// 00000yyy yyxxxxxx            110yyyyy      10xxxxxx
		*out = static_cast<std::uint8_t>(0b1100'0000u | (0b0001'1111u & (cp>>6)));
		++out;
		*out = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & cp));
		++out;
	} else if (sz == 3) {
		// zzzzyyyy yyxxxxxx            1110zzzz      10yyyyyy       10xxxxxx
		*out = static_cast<std::uint8_t>(0b1110'0000u | (0b0000'1111u & (cp>>12)));
		++out;
		*out = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & (cp>>6)));
		++out;
		*out = static_cast<std::uint8_t>(0b1000'0000u | (0b1011'1111u & cp));
		++out;
	} else if (sz == 4) {
		// 000uuuuu zzzzyyyy yyxxxxxx   11110uuu      10uuzzzz       10yyyyyy      10xxxxxx
		*out = static_cast<std::uint8_t>(0b1111'0000u | (0b0000'0111u & (cp>>18)));  // uuu
		++out;
		*out = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & (cp>>12)));  // uu'zzzz
		++out;
		*out = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & (cp>>6)));
		++out;
		*out = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & cp));
		++out;
	}

	return out;
}


// Undefined if cp is not a valid codepoint
template<typename OIt>
OIt to_utf16(std::uint32_t cp, OIt out) {
	// #Table 3-5. UTF-16 Bit Distribution
	// Scalar Value                UTF-16
	// xxxxxxxxxxxxxxxx            xxxxxxxxxxxxxxxx
	// 000uuuuuxxxxxxxxxxxxxxxx    110110wwwwxxxxxx 110111xxxxxxxxxx
	int sz = size_utf16_code_unit_seq_from_codepoint(cp);
	if (sz == 1) {
		*out = static_cast<std::uint16_t>(cp);
		++out;
	} else if (sz == 2) {
		std::uint16_t wwww = (cp>>16)-1;
		std::uint16_t xxxxxx = (cp>>10)&0b111111u;
		*out = static_cast<std::uint16_t>((0b110110u<<10) | (wwww<<6) | xxxxxx);
		++out;
		std::uint16_t xxxxxxxxxx = cp&0b1111111111u;
		*out = static_cast<std::uint16_t>((0b110111u<<10) | xxxxxxxxxx);
		++out;
	}

	return out;
}


