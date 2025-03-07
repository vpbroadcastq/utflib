#pragma once
#include "utflib.h"
#include "generic_iterator.h"
#include <cstdint>
#include <span>
#include <iterator>
#include <optional>

// Notes
// These iterators are constructed with the underlying range and know their start&end points; they do
// not need to be compared with some external sentinel to determine when iteration is finished.  Not
// sure how you would implement a design like that with variable-length data, at least if there needs
// to be error handling...  Actually it could be easialy done.  The user creates a one-past-the-end
// iterator @ p_end; just need an operator==.
// TODO:  See std::default_sentinel_t - some iterators _do_ know their ranges

// TODO:  Endianness:  utf32 & 16 need BE & LE variants
// Would you ever want to work with BE data on an LE system, or would you just convert when reading in
// and writing out?  Problem is you may not know the encoding and may need to inspect the data before
// knowing you need to swap the bytes.
// You need two iterators: a "normal" one and a byte-swapping one.  On an LE machine, if the normal one
// works, you know the encoding is LE.  On an LE machine, if the normal one does not but the 
// byte-swapping one does, you know the encoding is BE.  You don't need explicit BE/LE variants of each 
// iterator; just a "normal" one and a swapping one.  Then, define type aliases to each of these based
// on the endianness of the target() where the aliases carry _BE/_LE suffixes.
// The same thing needs to be done with the "view" classes utf16_codepoint, utf32_codepoint :/.


// The utfN_iterator iterators treat all ill-formed subsequences, no matter how long, and no matter
// their contents, as single errors.
using utf8_iterator = utf_iterator<utf8_customizer>;
using utf16_iterator = utf_iterator<utf16_customizer>;
using utf32_iterator = utf_iterator<utf32_customizer>;


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
	std::optional<utf8_codepoint> get() const;
	
	// This is the only get()ter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is effecient for it to also offer get().
	std::span<const std::uint8_t> get_underlying() const;
private:
	const std::uint8_t* m_p {};
	const std::uint8_t* m_pbeg {};
	const std::uint8_t* m_pend {};
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
	std::optional<utf16_codepoint> get() const;
	
	// This is the only get()ter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is effecient for it to also offer get().
	std::span<const std::uint16_t> get_underlying() const;
private:
	const std::uint16_t* m_p {};
	const std::uint16_t* m_pbeg {};
	const std::uint16_t* m_pend {};
};


// Treats all ill-formed subsequences, no matter how long, and no matter their contents, as single errors
class utf32_iterator_swapping {
public:
	utf32_iterator_swapping()=delete;
	explicit utf32_iterator_swapping(std::span<const std::uint32_t>);

	bool is_finished() const;
	bool at_start() const;

	bool go_next();  // false if it didn't go anywhere (=>is_finished() prior to the call)
	bool go_prev();  // false if it didn't go anywhere (=>at_start() prior to the call)

	std::optional<codepoint> get_codepoint() const;
	std::optional<utf32_codepoint_swapped> get() const;
	
	// This is the only get()ter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is efficient for it to also offer get().
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
	std::optional<utf32_codepoint> get() const;
	
	// This is the only get()ter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is efficient for it to also offer get().
	std::span<const std::uint32_t> get_underlying() const;
private:
	const std::uint32_t* m_p {};
	const std::uint32_t* m_pbeg {};
	const std::uint32_t* m_pend {};
};


// Every word not part of a valid code unit sequence is treated as an indivdual error
class utf32_iterator_alt_swapping {
public:
	utf32_iterator_alt_swapping()=delete;
	explicit utf32_iterator_alt_swapping(std::span<const std::uint32_t>);

	bool is_finished() const;
	bool at_start() const;

	bool go_next();  // false if it didn't go anywhere (=>is_finished() prior to the call)
	bool go_prev();  // false if it didn't go anywhere (=>at_start() prior to the call)

	std::optional<codepoint> get_codepoint() const;
	std::optional<utf32_codepoint_swapped> get() const;
	
	// This is the only get()ter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is efficient for it to also offer get().
	std::span<const std::uint32_t> get_underlying() const;
private:
	const std::uint32_t* m_p {};
	const std::uint32_t* m_pbeg {};
	const std::uint32_t* m_pend {};
};


