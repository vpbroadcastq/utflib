#pragma once
#include <cstdint>
#include <span>


// Table 3-7. Well-Formed UTF-8 Byte Sequences
// Code Points          First Byte    Second Byte    Third Byte    Fourth Byte
// U+0000..U+007F       00..7F
// U+0080..U+07FF       C2..DF        80..BF
// U+0800..U+0FFF       E0            A0..BF         80..BF
// U+1000..U+CFFF       E1..EC        80..BF         80..BF
// U+D000..U+D7FF       ED            80..9F         80..BF
// U+E000..U+FFFF       EE..EF        80..BF         80..BF
// U+10000..U+3FFFF     F0            90..BF         80..BF        80..BF
// U+40000..U+FFFFF     F1..F3        80..BF         80..BF        80..BF
// U+100000..U+10FFFF   F4            80..8F         80..BF        80..BF


// True if b falls in the range given in any row of the "First Byte" column of table 3-7, false
// otherwise.
bool is_valid_utf8_leading_byte(std::uint8_t b);

// True if tb falls in the range given in the "Second Byte" column for the row corresponding to the
// First Byte lb, false otherwise.  If lb is outside the range of any of the rows of the First Byte
// column of table 3-7 (that is, if lb is not a valid leading byte), the result is undefined; do not
// call this function with a value of tb for which is_valid_utf8_leading_byte returns false.
bool is_valid_utf8_second_byte(std::uint8_t tb, std::uint8_t lb);

// True if lb falls on [0x80, 0xBF], false otherwise.
bool is_valid_utf8_third_or_fourth_byte(std::uint8_t lb);

// True if b is on [0x80, 0xBF], false otherwise.  A byte in this range *might* be a valid
// trailing byte, but a byte outside this range can *never* be a valid trailing byte.  Although
// all third and fourth trailing bytes fall on [0x80, 0xBF], for certain code unit sequences,
// the second trailing byte is restricted to fall on [0xA0, 0xBF], [0x80, 0x9F], [0x90, 0xBF],
// or [0x80, 0x8F] depending on the leading byte; see Table 3-7.
bool is_utf8_trailing_byte(std::uint8_t b);

// From the first byte b of a multibyte sequence, computes the number of bytes in the sequence.
// If b is not a valid leading byte of a multibyte sequence the result is undefined.  Do not 
// call this function with a value of b for which is_valid_utf8_leading_byte returns false.
int size_utf8_multibyte_seq_from_leading_byte(std::uint8_t b);

// Returns the number of bytes required to encode the given codepoint cp as a utf8 byte sequence.
// The result is undefined if cp is not a valid codepoint.  Do not call this function with a value of
// cp for which is_valid_cp returns false.
int size_utf8_multibyte_seq_from_codepoint(std::uint32_t cp);

// Extracts the value bits from the leading byte of a potentially multibyte sequence.
// sz_multib == size_utf8_multibyte_seq_from_leading_byte(b); the result is undefined if sz_multib is incorrect
// or if b is not a valid leading byte (ie, if !is_valid_utf8_leading_byte(b)).
std::uint8_t payload_utf8_leading_byte(std::uint8_t b, int sz_multib);

// Extracts the value bits from a trailing byte of a multibyte sequence.  The result
// is undefined if !is_valid_trailing_byte(b).
std::uint8_t payload_utf8_trailing_byte(std::uint8_t b);

struct leading_byte_ptr_with_size {
	const std::uint8_t* p {};
	int sz {};
};
// Begins at the start of the span and seeks to the first valid leading byte.  None of the
// trailing bytes are validated.  The sequence implied by the leading byte could be invalid
// if any member of the trailing sequence is ill-formed.
// p==end <=> sz==0
// There is no meaningful "error" state different from "searched all the way to the end"
// Users should compare p to s.end(); if p<s.end(), sz~[1,4] && p+sz <= s.end()
leading_byte_ptr_with_size seek_to_first_utf8_leading_byte(std::span<const std::uint8_t> s);

// Gets the first valid utf8 byte sequence it finds starting at s.begin()
std::span<const std::uint8_t> seek_to_first_valid_utf8_sequence(std::span<const std::uint8_t> s);

bool begins_with_valid_utf8(std::span<const std::uint8_t> s);
// The span must contain exactly one codepoint and s.size()==size_utf8_multibyte_seq_from_codepoint_multibyte_seq_from_leading_byte(s[0])
bool is_valid_utf8_single_codepoint(std::span<const std::uint8_t> s);


bool is_valid_cp(std::uint32_t cp);

// Undefined if s is not a valid utf8 byte sequence
// Assumes that s.size() > 0 && s.size() >= size_utf8_multibyte_seq_from_codepoint_multibyte_seq_from_leading_byte(s[0])
// TODO:  Rename to to_codepoint?  to_unicode_scalar_value()?
std::uint32_t to_utf32(std::span<const std::uint8_t> s);


//
// UTF-16
//

// Table 3-5. UTF-16 Bit Distribution
// Scalar Value               UTF-16
// xxxxxxxxxxxxxxxx           xxxx'xxxx'xxxx'xxxx
// 000uuuuuxxxxxxxxxxxxxxxx   1101'10ww'wwxx'xxxx  1101'11xx'xxxx'xxxx

// Returns true if the given word is a valid codepoint; it follows that the word is not a member
// of a surrogate pair.  False otherwise.
// Valid codepoints not encoded in utf-16 by surrogate pairs fall on [0x0,0xD7FF] or [0xE000,0xFFFF].
bool is_valid_utf16_codepoint(std::uint16_t w);

// Returns true if the given word is valid as the leading word of a surrogate pair, false otherwise.
bool is_valid_utf16_surrogate_pair_leading(std::uint16_t w);

// Returns true if the given word is valid as the trailing word of a surrogate pair, false otherwise.
bool is_valid_utf16_surrogate_pair_trailing(std::uint16_t w);

bool is_valid_utf16_surrogate_pair(std::uint16_t lw, std::uint16_t tw);

std::uint16_t payload_leading_word_utf16_surrogate(std::uint16_t lw);
std::uint16_t payload_trailing_word_utf16_surrogate(std::uint16_t tw);

std::uint32_t utf16_to_codepoint_value(std::uint16_t w);
std::uint32_t utf16_to_codepoint_value(std::uint16_t lw, std::uint16_t tw);

// Returns the number of words required to encode the given codepoint cp as a utf16 uint16_t sequence.
// The result is undefined if cp is not a valid codepoint.  Do not call this function with a value of
// cp for which is_valid_cp returns false.
int size_utf16_code_unit_seq_from_codepoint(std::uint32_t);

// Gets the first valid utf16 word sequence it finds starting at s.begin().  The span encloses the
// word sequence corresponding to the single code point.  In an alternative design it could merely
// start at the first word of the codepont and have the same end as the input, but this discards
// information since it has to calculate the number of code units in the sequence (1 or 2) in order
// to validate it.
std::span<const std::uint16_t> seek_to_first_valid_utf16_sequence(std::span<const std::uint16_t> s);

// The span must contain exactly one codepoint:
// s.size()==1 && is_valid_utf16_codepoint(s[0])
// or
// s.size()==2 && is_valid_utf16_surrogate_pair(s[0],s[1])
bool is_valid_utf16_single_codepoint(std::span<const std::uint16_t> s);

void expect(bool, const char* = nullptr);
