#pragma once
#include <cstdint>
#include <span>
#include <filesystem>
#include <vector>
#include <concepts>
#include <ranges>


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

	friend class utf8_iterator;
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

	friend class utf16_iterator;
	friend class utf16_iterator_alt;
private:
	utf16_codepoint(const std::uint16_t*, const std::uint16_t*);
	explicit utf16_codepoint(std::span<const std::uint16_t>);

	std::span<const std::uint16_t> m_data;
};


// Value-semantic representation of a codepoint
class codepoint {
public:
	// == 0
	codepoint() = default;
	explicit codepoint(utf8_codepoint);
	explicit codepoint(utf16_codepoint);

	// TODO:  This does not need to be a template.  Let the user figure it out
	template<typename T>
	requires std::convertible_to<T,std::uint32_t>
	static std::optional<codepoint> to_codepoint(T val) noexcept {
		// ToDo:  If I am going to accept non-uint32_t types, need an is_valid_cp() that can work w/
		// (-) #'s as well as those > uint32_max.
		if (!is_valid_cp(static_cast<std::uint32_t>(val))) {
			return std::nullopt;
		}
		return codepoint(static_cast<std::uint32_t>(val));
	}

	std::uint32_t get() const noexcept;

	friend std::strong_ordering operator<=>(const codepoint&,const codepoint&) = default;
	friend class utf8_iterator;
	friend class utf16_iterator_alt;
	friend class utf8_iterator_alt;
	friend class utf16_iterator;
private:
	// Private because no validation is performed.  The value must be a valid codepoint.  Users should create
	// codepoints via the static member to_codepoint(T).
	explicit codepoint(std::uint32_t val) : m_val(val) {}

	// TODO:  Dubious?  The impl has to assume it's UTF-8.  Maybe potential users of this should just make 
	// the uint32_t conversion themselves and use the one above.  
	explicit codepoint(std::span<const std::uint8_t>);

	std::uint32_t m_val {};
};

