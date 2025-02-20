#include "utflib/utflib.h"
#include "utflib/byte_manip.h"
#include "utflib/low_level.h"
#include <span>
#include <cstdint>

//
// UTF-8 view
//
std::optional<utf8_codepoint> utf8_codepoint::to_utf8_codepoint(std::span<const std::uint8_t> s) {
	if (!is_valid_utf8_single_codepoint(s)) {
		return std::nullopt;
	}
	return utf8_codepoint(s);
}

utf8_codepoint::utf8_codepoint(const std::uint8_t* beg, const std::uint8_t* end) {
	m_data = std::span {beg, end};
}

utf8_codepoint::utf8_codepoint(std::span<const std::uint8_t> s) : m_data(s) {
	//...
}

std::span<const std::uint8_t>::iterator utf8_codepoint::begin() const {
	return m_data.begin();
}
std::span<const std::uint8_t>::iterator utf8_codepoint::end() const {
	return m_data.end();
}


//
// UTF-16 view
//
std::optional<utf16_codepoint> utf16_codepoint::to_utf16_codepoint(std::span<const std::uint16_t> s) {
	if (!is_valid_utf16_single_codepoint(s)) {
		return std::nullopt;
	}
	return utf16_codepoint(s);
}

utf16_codepoint::utf16_codepoint(const std::uint16_t* beg, const std::uint16_t* end) {
	m_data = std::span {beg, end};
}

utf16_codepoint::utf16_codepoint(std::span<const std::uint16_t> s) : m_data(s) {
	//...
}

std::span<const std::uint16_t>::iterator utf16_codepoint::begin() const {
	return m_data.begin();
}
std::span<const std::uint16_t>::iterator utf16_codepoint::end() const {
	return m_data.end();
}

//
// UTF-32 view
//
std::optional<utf32_codepoint> utf32_codepoint::to_utf32_codepoint(std::span<const std::uint32_t> s) {
	if (s.size()==1 && !is_valid_utf32_codepoint(s[0])) {
		return std::nullopt;
	}
	return utf32_codepoint(s);
}

utf32_codepoint::utf32_codepoint(const std::uint32_t* beg, const std::uint32_t* end) {
	m_data = std::span {beg, end};
}

utf32_codepoint::utf32_codepoint(std::span<const std::uint32_t> s) : m_data(s) {
	//...
}

std::span<const std::uint32_t>::iterator utf32_codepoint::begin() const {
	return m_data.begin();
}
std::span<const std::uint32_t>::iterator utf32_codepoint::end() const {
	return m_data.end();
}


//
// UTF-32 swapped view
//
std::optional<utf32_codepoint_swapped> utf32_codepoint_swapped::to_utf32_codepoint_swapped(std::span<const std::uint32_t> s) {
	if (s.size()==1 && !is_valid_utf32_codepoint(reverse_bytes(s[0]))) {
		return std::nullopt;
	}
	return utf32_codepoint_swapped(s);
}

utf32_codepoint_swapped::utf32_codepoint_swapped(const std::uint32_t* beg, const std::uint32_t* end) {
	m_data = std::span {beg, end};
}

utf32_codepoint_swapped::utf32_codepoint_swapped(std::span<const std::uint32_t> s) : m_data(s) {
	//...
}

std::span<const std::uint32_t>::iterator utf32_codepoint_swapped::begin() const {
	return m_data.begin();
}
std::span<const std::uint32_t>::iterator utf32_codepoint_swapped::end() const {
	return m_data.end();
}


//
// codepoint value type
//
codepoint::codepoint(utf8_codepoint u8) {
	int sz = u8.size();
	if (sz == 1) {
		m_val = payload_utf8_leading_byte(u8[0], sz);
	} else if (sz == 2) {
		m_val = payload_utf8_leading_byte(u8[0], sz);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(u8[1]);
	} else if (sz == 3) {
		m_val = payload_utf8_leading_byte(u8[0], sz);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(u8[1]);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(u8[2]);
	} else if (sz == 4) {
		m_val = payload_utf8_leading_byte(u8[0], sz);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(u8[1]);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(u8[2]);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(u8[3]);
	}
}

codepoint::codepoint(utf16_codepoint u16) {
	int sz = u16.size();
	if (sz == 1) {
		m_val = utf16_to_codepoint_value(u16[0]);
	} else if (sz == 2) {
		m_val = utf16_to_codepoint_value(u16[0],u16[1]);
	}
}

codepoint::codepoint(utf32_codepoint u32) {
	m_val = u32[0];
}

codepoint::codepoint(utf32_codepoint_swapped u32) {
	m_val = reverse_bytes(u32[0]);
}

// Private ctor; unchecked
codepoint::codepoint(std::span<const std::uint8_t> s) {
	int sz = s.size();
	if (sz == 1) {
		m_val = payload_utf8_leading_byte(s[0], sz);
	} else if (sz == 2) {
		m_val = payload_utf8_leading_byte(s[0], sz);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(s[1]);
	} else if (sz == 3) {
		m_val = payload_utf8_leading_byte(s[0], sz);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(s[1]);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(s[2]);
	} else if (sz == 4) {
		m_val = payload_utf8_leading_byte(s[0], sz);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(s[1]);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(s[2]);
		m_val <<= 6;
		m_val += payload_utf8_trailing_byte(s[3]);
	}
}

std::optional<codepoint> codepoint::to_codepoint(std::uint32_t val) noexcept {
	if (!is_valid_cp(val)) {
		return std::nullopt;
	}
	return codepoint(val);
}

std::uint32_t codepoint::get() const noexcept {
	return m_val;
}

