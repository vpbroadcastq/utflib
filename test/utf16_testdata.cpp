#include "utf16_testdata.h"

#include <vector>
#include <cstdint>
#include <array>
#include <optional>
#include <span>


std::span<utf16_single_word_testdata> get_valid_utf16_single_word_data() {
	static std::array<utf16_single_word_testdata,29> d {{
		// 1 dight
		{0x0u,0x0u}, {0x1u,0x1u}, {0x2u,0x2u}, {0xAu,0xAu}, {0xBu,0xBu}, {0xFu,0xFu},
		// 2 dight
		{0x10u,0x10u}, {0x11u,0x11u}, {0x21u,0x21u}, {0xACu,0xACu}, {0xBBu,0xBBu},
		{0xFFu,0xFFu},
		// 3 digit
		{0x100u,0x100u}, {0x111u,0x111u}, {0x211u,0x211u}, {0xACDu,0xACDu},
		{0xBBBu,0xBBBu}, {0xFFFu,0xFFFu},
		// 4 digit
		{0x1000u,0x1000u}, {0x1111u,0x1111u}, {0x2111u,0x2111u}, {0x8D63u,0x8D63u},
		{0xACDEu,0xACDEu}, {0xBBBBu,0xBBBBu}, {0xBCDEu,0xBCDEu},
		{0xD7FFu,0xD7FFu},  // Upper bound of first range
		{0xE000u,0xE000u},  // Lower bound of second range
		{0xFFFEu,0xFFFEu}, {0xFFFFu,0xFFFFu},
	}};
	return d;
}

// Invalid as single-word codepoints => either leading or trailing surrogates
std::span<utf16_single_word_testdata> get_invalid_utf16_single_word_data() {
	static std::array<utf16_single_word_testdata,11> d {{
		{0xD800u, std::nullopt},
		{0xD888u, std::nullopt}, {0xD8ABu, std::nullopt}, {0xD8FFu, std::nullopt},
		{0xD912u, std::nullopt}, {0xD9CCu, std::nullopt}, {0xD9FFu, std::nullopt},
		{0xDE00u, std::nullopt}, {0xDEABu, std::nullopt}, {0xDEFFu, std::nullopt},
		{0xDFFFu, std::nullopt},
	}};
	return d;
}


std::span<utf16_single_surrogate_testdata> get_utf16_leading_surrogate_test_data() {
	// <D800 DF02> corresponds to U+10302
	// 0x10302 == 0b0000'0001'0000'0011'0000'0010
	//            0b000u'uuuu'xxxx'xxxx'xxxx'xxxx
	//            0b0000'0001'0000'0011'0000'0010
	// Other examples:
	// U+10000 {U+D800, U+DC00}
	// U+10E6D {U+D803, U+DE6D}
	// U+1D11E {U+D834, U+DD1E}
	// U+10FFFF {U+DBFF, U+DFFF}
	static std::array<utf16_single_surrogate_testdata,8> d {{
		//
		// Valid
		//

		// 0xD800 == 0b1101'1000'0000'0000
		{0xD800u, 0b1000000u},
		//
		// Invalid
		// (Could be valid as a trailing surrogate or could be a single-word codepoint)
		//
		// Valid single-word (invalid as surrogate)
		{0x0u, std::nullopt},
		{0xD7FFu, std::nullopt},  // Upper bound of first range of valid single-word codepoints
		{0xE000u, std::nullopt},  // Lower bound of second range of valid single-word codepoints
		// Valid _trailing_ surrogates
		{0xDC00u, std::nullopt},
		{0xDE6Du, std::nullopt},
		{0xDD1E, std::nullopt},
		{0xDFFF, std::nullopt},
	}};
	return d;
}
 
std::span<utf16_single_surrogate_testdata> get_utf16_trailing_surrogate_test_data() {
	// <D800 DF02> corresponds to U+10302
	// 0x10302 == 0b0000'0001'0000'0011'0000'0010
	//            0b0000'0000'0000'00xx'xxxx'xxxx
	//            0b0000'0000'0000'0011'0000'0010
	// Other examples:
	// U+10000 {U+D800, U+DC00}
	// U+10E6D {U+D803, U+DE6D}
	// U+1D11E {U+D834, U+DD1E}
	// U+10FFFF {U+DBFF, U+DFFF}
	static std::array<utf16_single_surrogate_testdata,8> d {{
		//
		// Valid
		//
		{0xDF02u, 0b11'0000'0010u},
		//
		// Invalid
		// (Could be valid as a trailing surrogate or could be a single-word codepoint)
		//
		// Valid single-word (invalid as surrogate)
		{0x0u, std::nullopt},
		{0xD7FFu, std::nullopt},  // Upper bound of first range of valid single-word codepoints
		{0xE000u, std::nullopt},  // Lower bound of second range of valid single-word codepoints
		// Valid _leading_ surrogates
		{0xD800u, std::nullopt},
		{0xD803u, std::nullopt},
		{0xD834u, std::nullopt},
		{0xDBFFu, std::nullopt},
	}};
	return d;
}

std::span<utf16_valid_surrogate_pair_testdata> get_valid_utf16_surrogate_pair_test_data() {
	static std::array<utf16_valid_surrogate_pair_testdata,14> d {{
		{0xD800u, 0xDC00u, 0x10000u},
		{0xD803u, 0xDE6Du, 0x10E6Du},
		{0xD834u, 0xDD1Eu, 0x1D11Eu},
		{0xDBFFu, 0xDFFFu, 0x10FFFFu},
		// Confirmed using Babelpad
		{0xD8DAu, 0xDE18u, 0x46A18u},
		{0xDA05u, 0xDF57u, 0x91757u},
		{0xDB3Fu, 0xDD48u, 0xDFD48u},
		{0xDBD1u, 0xDC0Fu, 0x10440Fu},
		{0xD912u, 0xDC4Du, 0x5484Du},
		{0xDBCAu, 0xDDAEu, 0x1029AEu},
		{0xDBCFu, 0xDEB3u, 0x103EB3u},
		{0xD9E9u, 0xDF3Fu, 0x8A73Fu},
		{0xDBFAu, 0xDDA0u, 0x10E9A0u},
		{0xDA09u, 0xDE4Au, 0x9264Au},
	}};
	return d;
}


std::span<utf16_codepoint_and_code_unit_seq_len_testdata> get_utf16_valid_codepoint_to_codeunit_seq_len_testdata() {
	static std::array<utf16_codepoint_and_code_unit_seq_len_testdata,11> d {{
		// 1
		{0x0u,1},{0xD7FFu,1},{0xE000u,1},{0xFFFFu,1},
		{0x8D63u,1},
		// 2
		{0x10000u,2}, {0x10FFFF,2},
		{0x91757u,2}, {0x46A18,2},
		{0x5484Du,2}, {0xDFD48,2}
	}};
	return d;
}


//
// Sequences
//
//struct testdata_utf16_utf32_sequences {
//	std::vector<std::uint16_t> utf16;
//	std::vector<std::uint32_t> utf32;
//	std::vector<std::uint32_t> utf32_alt;  // Alternate substitution procedure
//};
std::span<testdata_utf16_utf32_sequences> get_valid_utf16_sequences() {
	static std::array<testdata_utf16_utf32_sequences,5> d {{
		{
			// Example from the unicode standard
			{0x004Du, 0x0430u, 0x4E8Cu, 0xD800u, 0xDF02u},
			{0x004Du, 0x0430u, 0x4E8Cu, 0x10302u},
			{}
		},
		{
			{0x3FD9u, 0x00A0u, 0xD80Du, 0xDD50u, 0xDA1Fu, 0xDDC9u, 0xDACBu, 0xDD55u, 0x4134u, 0xF936u},
			{0x3FD9u, 0x00A0u, 0x13550u,         0x97DC9u,         0xC2D55u,         0x4134u, 0xF936u},
			{}
		},
		{
			{0xD80Du, 0xDD50u, 0xDA1Fu, 0xDDC9u, 0xDACBu, 0xDD55u, 0x3FD9u, 0x00A0u, 0x4134u, 0xF936u},
			{0x13550u,         0x97DC9u,         0xC2D55u,         0x3FD9u, 0x00A0u, 0x4134u, 0xF936u},
			{}
		},
		{
			{0x3FD9u, 0x00A0u, 0x4134u, 0xF936u, 0xD80Du, 0xDD50u, 0xDA1Fu, 0xDDC9u, 0xDACBu, 0xDD55u},
			{0x3FD9u, 0x00A0u, 0x4134u, 0xF936u, 0x13550u,         0x97DC9u,         0xC2D55u,       },
			{}
		},
		{
			{0x00A0u, 0x4134u, 0xF936u, 0xD80Du, 0xDD50u, 0x3FD9u, 0xDA1Fu, 0xDDC9u, 0xDACBu, 0xDD55u},
			{0x00A0u, 0x4134u, 0xF936u, 0x13550u,         0x3FD9u, 0x97DC9u,         0xC2D55u,       },
			{}
		},
	}};
	return d;
}

std::span<testdata_utf16_utf32_sequences> get_invalid_utf16_sequences() {
	static std::array<testdata_utf16_utf32_sequences,7> d {{
		{
			// Completely invalid - no valid codepoints
			{0xDD55u, 0xDD50u, 0xDDC9u, 0xD80Du, 0xDA1Fu, 0xDACBu, 0xDACBu},
			{0xFFFDu},
			{0xFFFDu, 0xFFFDu, 0xFFFDu, 0xFFFDu, 0xFFFDu, 0xFFFDu, 0xFFFDu}
		},
		{ //                   lb & tb reversed
			{0x3FD9u, 0x00A0u, 0xDD50u, 0xD80Du, 0xDA1Fu, 0xDDC9u, 0xDACBu, 0xDD55u, 0x4134u, 0xF936u},
			{0x3FD9u, 0x00A0u, 0xFFFDu,          0x97DC9u,         0xC2D55u,         0x4134u, 0xF936u},
			{0x3FD9u, 0x00A0u, 0xFFFDu, 0xFFFDu, 0x97DC9u,         0xC2D55u,         0x4134u, 0xF936u}
		},
		{ //                   tb,      lb,      lb,      lb
			{0x3FD9u, 0x00A0u, 0xDD50u, 0xD80Du, 0xDA1Fu, 0xDA1Fu, 0xDACBu, 0xDD55u, 0x4134u, 0xF936u},
			{0x3FD9u, 0x00A0u, 0xFFFDu,                            0xC2D55u,         0x4134u, 0xF936u},
			{0x3FD9u, 0x00A0u, 0xFFFDu, 0xFFFDu, 0xFFFDu, 0xFFFDu, 0xC2D55u,         0x4134u, 0xF936u},
		},
		{ //                   tb,      lb,               lb,      lb
			{0x3FD9u, 0x00A0u, 0xDD50u, 0xD80Du, 0x0000u, 0xDA1Fu, 0xDA1Fu, 0xDACBu, 0xDD55u, 0x4134u, 0xF936u},
			{0x3FD9u, 0x00A0u, 0xFFFDu,          0x0000u, 0xFFFDu,          0xC2D55u,         0x4134u, 0xF936u},
			{0x3FD9u, 0x00A0u, 0xFFFDu, 0xFFFDu, 0x0000u, 0xFFFDu, 0xFFFDu, 0xC2D55u,         0x4134u, 0xF936u}
		},
		{ // tb,      lb,               lb,      lb
			{0xDD50u, 0xD80Du, 0x0000u, 0xDA1Fu, 0xDA1Fu, 0xDACBu, 0xDD55u, 0x4134u, 0xF936u, 0x3FD9u, 0x00A0u},
			{0xFFFDu,          0x0000u, 0xFFFDu,          0xC2D55u,         0x4134u, 0xF936u, 0x3FD9u, 0x00A0u},
			{0xFFFDu, 0xFFFDu, 0x0000u, 0xFFFDu, 0xFFFDu, 0xC2D55u,         0x4134u, 0xF936u, 0x3FD9u, 0x00A0u}
		},
		{ //                                                       tb,      lb,      lb,      lb
			{0x3FD9u, 0x00A0u, 0xDACBu, 0xDD55u, 0x4134u, 0xF936u, 0xDD50u, 0xD80Du, 0xDA1Fu, 0xDA1Fu},
			{0x3FD9u, 0x00A0u, 0xC2D55u,         0x4134u, 0xF936u, 0xFFFDu,                          },
			{0x3FD9u, 0x00A0u, 0xC2D55u,         0x4134u, 0xF936u, 0xFFFDu, 0xFFFDu, 0xFFFDu, 0xFFFDu}
		},
		{ //                                     tb,      lb,      lb       lb
			{0x0000u, 0xDACBu, 0xDD55u, 0x4134u, 0xDD50u, 0xD80Du, 0xDA1Fu, 0xDA1Fu, 0xF936u, 0x3FD9u, 0x00A0u},
			{0x0000u, 0xC2D55u,         0x4134u, 0xFFFDu,                            0xF936u, 0x3FD9u, 0x00A0u},
			{0x0000u, 0xC2D55u,         0x4134u, 0xFFFDu, 0xFFFDu, 0xFFFDu, 0xFFFDu, 0xF936u, 0x3FD9u, 0x00A0u}
		},
	}};
	return d;
}