#include "utflib/encoders.h"

#include "utflib/utflib.h"
#include "utflib/low_level.h"
#include <span>
#include <cstdint>
#include <optional>

//
// utf8 generator
//
utf8_generator::utf8_generator(codepoint cp) noexcept {
	std::uint32_t val = cp.get();
	m_curr_idx = 0;
	m_sz = size_utf8_multibyte_seq_from_codepoint(val);

	// Table 3-6. UTF-8 Bit Distribution
	// Scalar Value                 First Byte    Second Byte    Third Byte    Fourth Byte
	// 00000000 0xxxxxxx            0xxxxxxx
	// 00000yyy yyxxxxxx            110yyyyy      10xxxxxx
	// zzzzyyyy yyxxxxxx            1110zzzz      10yyyyyy       10xxxxxx
	// 000uuuuu zzzzyyyy yyxxxxxx   11110uuu      10uuzzzz       10yyyyyy      10xxxxxx
	if (m_sz == 1) {
		m_u8[0] = static_cast<std::uint8_t>(val);
		m_u8[1] = 0xFFu;
		m_u8[2] = 0xFFu;
		m_u8[3] = 0xFFu;
	} else if (m_sz == 2) {
		// 00000yyy yyxxxxxx            110yyyyy      10xxxxxx
		m_u8[0] = static_cast<std::uint8_t>(0b1100'0000u | (0b0001'1111u & (val>>6)));
		m_u8[1] = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & val));
		m_u8[2] = 0xFFu;
		m_u8[3] = 0xFFu;
	} else if (m_sz == 3) {
		// zzzzyyyy yyxxxxxx            1110zzzz      10yyyyyy       10xxxxxx
		m_u8[0] = static_cast<std::uint8_t>(0b1110'0000u | (0b0000'1111u & (val>>12)));
		m_u8[1] = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & (val>>6)));
		m_u8[2] = static_cast<std::uint8_t>(0b1000'0000u | (0b1011'1111u & val));
		m_u8[3] = 0xFFu;
	} else { // if (m_sz == 4) {
		// 000uuuuu zzzzyyyy yyxxxxxx   11110uuu      10uuzzzz       10yyyyyy      10xxxxxx
		m_u8[0] = static_cast<std::uint8_t>(0b1111'0000u | (0b0000'0111u & (val>>18)));  // uuu
		m_u8[1] = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & (val>>12)));  // uu'zzzz
		m_u8[2] = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & (val>>6)));
		m_u8[3] = static_cast<std::uint8_t>(0b1000'0000u | (0b0011'1111u & val));
	}
}

bool utf8_generator::is_finished() const noexcept {
	return m_curr_idx==m_sz;
}

std::uint8_t utf8_generator::get() const noexcept {
	return m_u8[m_curr_idx];
}

bool utf8_generator::go_next() noexcept {
	if (m_curr_idx==m_sz) {
		return false;
	}
	++m_curr_idx;
	return true;
}

void utf8_generator::reset() noexcept {
	m_curr_idx = 0;
}


//
// utf16 generator
//
utf16_generator::utf16_generator(codepoint cp) noexcept {
	std::uint32_t val = cp.get();
	m_curr_idx = 0;
	m_sz = size_utf16_code_unit_seq_from_codepoint(val);

	// #Table 3-5. UTF-16 Bit Distribution
	// Scalar Value                UTF-16
	// xxxxxxxxxxxxxxxx            xxxxxxxxxxxxxxxx
	// 000uuuuuxxxxxxxxxxxxxxxx    110110wwwwxxxxxx 110111xxxxxxxxxx
	if (m_sz == 1) {
		m_u16[0] = static_cast<std::uint16_t>(val);
	} else if (m_sz == 2) {
		std::uint16_t wwww = (val>>16)-1;
		std::uint16_t xxxxxx = (val>>10)&0b111111u;
		m_u16[0] = static_cast<std::uint16_t>((0b110110u<<10) | (wwww<<6) | xxxxxx);
		std::uint16_t xxxxxxxxxx = val&0b1111111111u;
		m_u16[1] = static_cast<std::uint16_t>((0b110111u<<10) | xxxxxxxxxx);
	}
}

bool utf16_generator::is_finished() const noexcept {
	return m_curr_idx==m_sz;
}

std::uint16_t utf16_generator::get() const noexcept {
	return m_u16[m_curr_idx];
}

bool utf16_generator::go_next() noexcept {
	if (m_curr_idx==m_sz) {
		return false;
	}
	++m_curr_idx;
	return true;
}

void utf16_generator::reset() noexcept {
	m_curr_idx = 0;
}




