#pragma once
#include "utflib.h"
#include <cstdint>
#include <span>

// Notes
// These iterators are constructed with the underlying range and know their start&end points; they do
// not need to be compared with some external sentinel to determine when iteration is finished.  Not
// sure how you would implement a design like that with variable-length data, at least if there needs
// to be error handling.

// Treats all ill-formed subsequences, no matter how long, and no matter their contents, as single errors
// TODO:  Behavior of the getters is probably not right when is_finished() or when m_p is on the last
//        valid subsequence but there is a trailing invalid subsequence.
class utf8_iterator {
public:
	utf8_iterator()=delete;
	explicit utf8_iterator(std::span<const std::uint8_t>);

	bool is_finished() const;
	bool at_start() const;

	bool go_next();  // false if it didn't go anywhere (=>is_finished() prior to the call)
	bool go_prev();  // false if it didn't go anywhere (=>at_start() prior to the call)

	std::optional<codepoint> get_codepoint() const;
	std::optional<utf8_codepoint> get_utf8() const;
	
	// This is the only getter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is effecient for it to also offer get_utf8().
	std::span<const std::uint8_t> get_underlying() const;
private:
	const std::uint8_t* m_p {};
	const std::uint8_t* m_pbeg {};
	const std::uint8_t* m_pend {};
};


// "Only when a sequence of two or three bytes is a truncated version of a sequence which is
// otherwise well-formed to that point, is more than one byte replaced with a single U+FFFD,
// as shown in Table 3-11."
// 
// Table 3-11. U+FFFD for Truncated Sequences
// Bytes     E1	80   E2	    F0 91 92   F1 BF   41
// Output    FFFD    FFFD   FFFD       FFFD    0041
class utf8_iterator_alt {
public:
	utf8_iterator_alt()=delete;
	explicit utf8_iterator_alt(std::span<const std::uint8_t>);

	bool is_finished() const;
	bool at_start() const;

	bool go_next();  // false if it didn't go anywhere (=>is_finished() prior to the call)
	bool go_prev();  // false if it didn't go anywhere (=>at_start() prior to the call)

	std::optional<codepoint> get_codepoint() const;
	std::optional<utf8_codepoint> get_utf8() const;
	
	// This is the only getter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is effecient for it to also offer get_utf8().
	std::span<const std::uint8_t> get_underlying() const;
private:
	const std::uint8_t* m_p {};
	const std::uint8_t* m_pbeg {};
	const std::uint8_t* m_pend {};
};


// Treats all ill-formed subsequences, no matter how long, and no matter their contents, as single errors
class utf16_iterator {
public:
	utf16_iterator()=delete;
	explicit utf16_iterator(std::span<const std::uint16_t>);

	bool is_finished() const;
	bool at_start() const;

	bool go_next();  // false if it didn't go anywhere (=>is_finished() prior to the call)
	bool go_prev();  // false if it didn't go anywhere (=>at_start() prior to the call)

	std::optional<codepoint> get_codepoint() const;
	std::optional<utf16_codepoint> get_utf16() const;
	
	// This is the only getter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is effecient for it to also offer get_utf8().
	std::span<const std::uint16_t> get_underlying() const;
private:
	const std::uint16_t* m_p {};
	const std::uint16_t* m_pbeg {};
	const std::uint16_t* m_pend {};
};


// Every word not part of a valid code unit sequence is treated as an indivdual error
class utf16_iterator_alt {
public:
	utf16_iterator_alt()=delete;
	explicit utf16_iterator_alt(std::span<const std::uint16_t>);

	bool is_finished() const;
	bool at_start() const;

	bool go_next();  // false if it didn't go anywhere (=>is_finished() prior to the call)
	bool go_prev();  // false if it didn't go anywhere (=>at_start() prior to the call)

	std::optional<codepoint> get_codepoint() const;
	std::optional<utf16_codepoint> get_utf16() const;
	
	// This is the only getter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is effecient for it to also offer get_utf8().
	std::span<const std::uint16_t> get_underlying() const;
private:
	const std::uint16_t* m_p {};
	const std::uint16_t* m_pbeg {};
	const std::uint16_t* m_pend {};
};



// Treats all ill-formed subsequences, no matter how long, and no matter their contents, as single errors
class utf32_iterator {
public:
	utf32_iterator()=delete;
	explicit utf32_iterator(std::span<const std::uint32_t>);

	bool is_finished() const;
	bool at_start() const;

	bool go_next();  // false if it didn't go anywhere (=>is_finished() prior to the call)
	bool go_prev();  // false if it didn't go anywhere (=>at_start() prior to the call)

	std::optional<codepoint> get_codepoint() const;
	std::optional<utf32_codepoint> get_utf32() const;
	
	// This is the only getter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is effecient for it to also offer get_utf8().
	std::span<const std::uint32_t> get_underlying() const;
private:
	const std::uint32_t* m_p {};
	const std::uint32_t* m_pbeg {};
	const std::uint32_t* m_pend {};
};


// Every word not part of a valid code unit sequence is treated as an indivdual error
class utf32_iterator_alt {
public:
	utf32_iterator_alt()=delete;
	explicit utf32_iterator_alt(std::span<const std::uint32_t>);

	bool is_finished() const;
	bool at_start() const;

	bool go_next();  // false if it didn't go anywhere (=>is_finished() prior to the call)
	bool go_prev();  // false if it didn't go anywhere (=>at_start() prior to the call)

	std::optional<codepoint> get_codepoint() const;
	std::optional<utf32_codepoint> get_utf32() const;
	
	// This is the only getter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is effecient for it to also offer get_utf8().
	std::span<const std::uint32_t> get_underlying() const;
private:
	const std::uint32_t* m_p {};
	const std::uint32_t* m_pbeg {};
	const std::uint32_t* m_pend {};
};


