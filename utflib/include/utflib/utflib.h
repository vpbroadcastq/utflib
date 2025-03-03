#pragma once
#include <cstdint>
#include <span>
#include <filesystem>
#include <vector>
#include <concepts>
#include <ranges>


// TODO:  Detect encoding

// A non-owning view of a well-formed utf8 code unit sequence encoding exactly one codepoint
// TODO:  Templated on the underlying datatype?  Should I allow T's other than std::uint8_t?
// TODO:  utf8_code_unit_sequence?  utf8_encoded_codepoint?  utf8_view?
// TODO:  Is this useful?  Since it does ! allow mutation, it takes up more space than the actual
//        byte sequence encoding the codepoint would.
class utf8_codepoint : public std::ranges::view_interface<utf8_codepoint> {
public:
	utf8_codepoint() = delete;
	static std::optional<utf8_codepoint> to_utf8_codepoint(std::span<const std::uint8_t> s);

	std::span<const std::uint8_t>::iterator begin() const;
	std::span<const std::uint8_t>::iterator end() const;

	// std::ranges::view_interface generates empty() but a utf8_codepoint never points to an
	// empty byte sequence.  empty() is always false, so the impl below is more effecient.
	// TODO:  Hmmmmmmmmm is this a good idea?  Base-class methods will not defer to this, right?
	constexpr bool empty() const {
		return false;
	}

	template<typename T>
	friend class utf_iterator;
	//friend class utf8_iterator;
	friend class utf8_iterator_alt;
private:
	utf8_codepoint(const std::uint8_t*, const std::uint8_t*);
	explicit utf8_codepoint(std::span<const std::uint8_t>);

	std::span<const std::uint8_t> m_data;
};


// A non-owning view of a well-formed utf-16 code unit sequence encoding exactly one codepoint
// TODO:  Templated on the underlying datatype?  Should I allow T's other than std::uint16_t?
// TODO:  utf16_code_unit_sequence?  utf16_encoded_codepoint?  utf16_view?
// TODO:  Is this useful?  Since it does ! allow mutation, it takes up more space than the actual
//        byte sequence encoding the codepoint would.
class utf16_codepoint : public std::ranges::view_interface<utf16_codepoint> {
public:
	utf16_codepoint() = delete;
	static std::optional<utf16_codepoint> to_utf16_codepoint(std::span<const std::uint16_t> s);

	std::span<const std::uint16_t>::iterator begin() const;
	std::span<const std::uint16_t>::iterator end() const;

	// std::ranges::view_interface generates empty() but a utf16_codepoint never points to an
	// empty byte sequence.  empty() is always false, so the impl below is more effecient.
	// TODO:  Hmmmmmmmmm is this a good idea?  Base-class methods will not defer to this, right?
	constexpr bool empty() const {
		return false;
	}

	// TODO:  Here and elsewhere, only the appropriate specialization should be a friend
	template<typename T>
	friend class utf_iterator;
	//friend class utf16_iterator;
	friend class utf16_iterator_alt;
private:
	utf16_codepoint(const std::uint16_t*, const std::uint16_t*);
	explicit utf16_codepoint(std::span<const std::uint16_t>);

	std::span<const std::uint16_t> m_data;
};


// A non-owning view of a well-formed utf-32 code unit sequence encoding exactly one codepoint
// TODO:  Templated on the underlying datatype?  Should I allow T's other than std::uint16_t?
// TODO:  utf32_code_unit_sequence?  utf32_encoded_codepoint?  utf32_view?
// TODO:  Is this useful?  Since it does ! allow mutation, it takes up more space than the actual
//        byte sequence encoding the codepoint would.
class utf32_codepoint : public std::ranges::view_interface<utf32_codepoint> {
public:
	utf32_codepoint() = delete;
	static std::optional<utf32_codepoint> to_utf32_codepoint(std::span<const std::uint32_t> s);

	std::span<const std::uint32_t>::iterator begin() const;
	std::span<const std::uint32_t>::iterator end() const;

	// std::ranges::view_interface generates empty() but a utf16_codepoint never points to an
	// empty byte sequence.  empty() is always false, so the impl below is more effecient.
	// TODO:  Hmmmmmmmmm is this a good idea?  Base-class methods will not defer to this, right?
	constexpr bool empty() const {
		return false;
	}

	friend class utf32_iterator;
	friend class utf32_iterator_swapping;
	friend class utf32_iterator_alt;
private:
	utf32_codepoint(const std::uint32_t*, const std::uint32_t*);
	explicit utf32_codepoint(std::span<const std::uint32_t>);

	std::span<const std::uint32_t> m_data;
};


// A non-owning view of a well-formed utf-32 code unit sequence encoding exactly one codepoint
// TODO:  Templated on the underlying datatype?  Should I allow T's other than std::uint16_t?
// TODO:  utf32_code_unit_sequence?  utf32_encoded_codepoint?  utf32_view?
// TODO:  Is this useful?  Since it does ! allow mutation, it takes up more space than the actual
//        byte sequence encoding the codepoint would.
class utf32_codepoint_swapped : public std::ranges::view_interface<utf32_codepoint_swapped> {
public:
	utf32_codepoint_swapped() = delete;
	static std::optional<utf32_codepoint_swapped> to_utf32_codepoint_swapped(std::span<const std::uint32_t> s);

	std::span<const std::uint32_t>::iterator begin() const;
	std::span<const std::uint32_t>::iterator end() const;

	constexpr bool empty() const {
		return false;
	}

	friend class utf32_iterator;  // TODO:  Needed?
	friend class utf32_iterator_swapping;
	friend class utf32_iterator_alt;  // TODO:  Needed?
	friend class utf32_iterator_alt_swapping;
private:
	utf32_codepoint_swapped(const std::uint32_t*, const std::uint32_t*);
	explicit utf32_codepoint_swapped(std::span<const std::uint32_t>);

	std::span<const std::uint32_t> m_data;
};


// TODO:  My vocabulary is off:  "Because surrogate code points are not included in the set of Unicode
// scalar values, UTF-32 code units in the range 0000D80016..0000DFFF16 are ill-formed."  Unpaired
// surrogates _are_ valid "codepoints."  Also, "In the Unicode Standard, the codespace consists of the
// integers from 0 to 10FFFF16, comprising 1,114,112 code points available for assigning the repertoire
// of abstract characters.  Also, Table 2-3. Types of Code Points refers to surrogates as codepoints and
// assigns them a category.  A better name might be scalar_value?  However, the caption for Fig 2-11
// appears to contradict this language and uses the term "codepoint" the way i am using it here.  But
// then, "As for all of the Unicode encoding forms, UTF-32 is restricted to representation of code points
// in the ranges 0..D7FF16 and E00016..10FFFF16—that is, Unicode scalar values."  And, "The value of each
// UTF-32 code unit corresponds exactly to the Unicode code point value. This situation differs
// significantly from that for UTF-16 and especially UTF-8, where the code unit values often change
// unrecognizably from the code point value."   ... So are unpaired surrogates codepoints or not???
// Pretty sure surrogates are codepoints:  "Surrogates Area. The Surrogates Area contains only surrogate
// code points and no encoded characters. See Section 23.6, Surrogates Area, for more details."

// Value-semantic representation of a codepoint
class codepoint {
public:
	// == 0
	codepoint() = default;
	// TODO:  Do these make sense?  Maybe the views should know how to extract their own codepoint values
	explicit codepoint(utf8_codepoint);
	explicit codepoint(utf16_codepoint);
	explicit codepoint(utf32_codepoint);
	explicit codepoint(utf32_codepoint_swapped);

	static std::optional<codepoint> to_codepoint(std::uint32_t val) noexcept;

	std::uint32_t get() const noexcept;

	friend std::strong_ordering operator<=>(const codepoint&,const codepoint&) = default;
	template<typename T>
	friend class utf_iterator;
	//friend class utf8_iterator;
	friend class utf8_iterator_alt;
	friend class utf16_iterator_alt;
	//friend class utf16_iterator;
	friend class utf32_iterator_alt;
	friend class utf32_iterator;
	friend class utf32_iterator_swapping;
	friend class utf32_iterator_alt_swapping;
private:
	// Private because no validation is performed.  The value must be a valid codepoint.  Users should create
	// codepoints via the static member to_codepoint(T).
	explicit codepoint(std::uint32_t val) : m_val(val) {}

	// TODO:  Dubious?  The impl has to assume it's UTF-8.  Maybe potential users of this should just make 
	// the uint32_t conversion themselves and use the one above.  
	explicit codepoint(std::span<const std::uint8_t>);

	// Assumes valid utf16
	explicit codepoint(std::span<const std::uint16_t>);

	std::uint32_t m_val {};
};






