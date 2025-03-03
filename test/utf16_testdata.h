#pragma once
#include <span>
#include <cstdint>
#include <optional>
#include <vector>

// Examples of single utf-16 code units and their corresponding codepoint.  The "valid" set are those
// codepoints that can be encoded in utf-16 as a single code unit.  The "invalid" set are those code units
// that form only one half of a surrogate pair.
struct utf16_single_word_testdata {
	std::uint16_t w {};
	std::optional<std::uint32_t> codepoint {};  // Empty => w !valid as a single-word codepoint
};
std::span<utf16_single_word_testdata> get_valid_utf16_single_word_data();

// Invalid as single-word codepoints => either leading or trailing surrogates
std::span<utf16_single_word_testdata> get_invalid_utf16_single_word_data();


// TODO
// This needs to be redone in a simpler way.  I don't need a special type.  I just need a
// span<uint16_t> get_valid_leading_surrogates()
// span<uint16_t> get_invalid_leading_surrogates()
// span<uint16_t> get_valid_trailing_surrogates()
// span<uint16_t> get_invalid_trailing_surrogates()
// and perhaps similar for non-surrogate words.  Forget() testing the payload functions; they're
// weird anyway.
// If w is invalid as surrogate (invalid as leading and invalid as trailing), payload will be
// empty, otherwise payload will be populated with the leading or trailing payload as appropriate.
struct utf16_single_surrogate_testdata {
	std::uint16_t w {};
	std::optional<std::uint16_t> payload {};  // Empty => w !valid as a single-word codepoint
};
std::span<utf16_single_surrogate_testdata> get_utf16_leading_surrogate_test_data();
std::span<utf16_single_surrogate_testdata> get_utf16_trailing_surrogate_test_data();


struct utf16_valid_surrogate_pair_testdata {
	std::uint16_t lw {};
	std::uint16_t tw {};
	std::uint32_t cp {};
};
std::span<utf16_valid_surrogate_pair_testdata> get_valid_utf16_surrogate_pair_test_data();

// Codepoints and the corresponding number of utf-16 code units required to encode it (1 or 2).
// The size field is an optional to indicate entries where the codepoint is invalid.
//size_utf16_code_unit_seq_from_codepoint
struct utf16_codepoint_and_code_unit_seq_len_testdata {
	std::uint32_t codepoint {};
	std::optional<int> size {};  // If !empty, always 1 or 2
};
std::span<utf16_codepoint_and_code_unit_seq_len_testdata> get_utf16_valid_codepoint_to_codeunit_seq_len_testdata();



//
// Sequences
//

// Valid and invalid utf-16 sequences and their corresponding utf-32 translations.  The utf32 and
// utf32_alt fields are two diffferent substitution procedures for invalid subsequences.
struct testdata_utf16_utf32_sequences {
	std::vector<std::uint16_t> utf16;
	std::vector<std::uint32_t> utf32;
	std::vector<std::uint32_t> utf32_alt;  // Alternate substitution procedure
};
// These sequences contain no invalid subsequences
std::span<testdata_utf16_utf32_sequences> get_valid_utf16_sequences();

// These sequences contain invalid subsequences
std::span<testdata_utf16_utf32_sequences> get_invalid_utf16_sequences();


