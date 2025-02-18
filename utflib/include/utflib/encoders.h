#pragma once
#include <array>
#include <cstdint>
#include "low_level.h"
#include "utflib.h"

// TODO:  decoders.h/.cpp for reading in files; handle byte-order issues
// TODO:  A single cp is just a special case of a range containing a single cp.  These functions should
//        accept ranges, not single cp's.

// TODO:  These things should take codepoints, not the underlying int types
// TODO:  Endianness
// TODO:  Replace get() w/ get_all()

// Notes:  -.get_all() is safer b/c it will always write out all the code units.  Users using .get() with a
//           manual loop have to be careful to run the loop until .is_finished()
//         -"Generators" is not a good name?  These are really value-semantic representations of utfN
//          codepoints; they are the value versions of the "views" in utflib.h (utf8_codepoint,
//          utf16_codepoint, etc).  They have an implicit endianness.

// For a given codepoint, generates in order the bytes in the utf8 representation
class utf8_generator {
public:
	utf8_generator()=delete;
	utf8_generator(codepoint) noexcept;

	bool is_finished() const noexcept;
	std::uint8_t get() const noexcept;
	bool go_next() noexcept;
	void reset() noexcept;

	// Unrolls the loop that the user would normally have to write
	// TODO:  Constrain
	template<typename OIt>
	OIt get_all(OIt out) const {
		if (m_sz == 1) {
			*out++ = m_u8[0];
		} else if (m_sz == 2) {
			*out++ = m_u8[0];
			*out++ = m_u8[1];
		} else if (m_sz == 3) {
			*out++ = m_u8[0];
			*out++ = m_u8[1];
			*out++ = m_u8[2];
		} else { //if (m_sz == 4)
			*out++ = m_u8[0];
			*out++ = m_u8[1];
			*out++ = m_u8[2];
			*out++ = m_u8[3];
		}
		return out;
	}

private:
	std::array<std::uint8_t,4> m_u8;
	std::int16_t m_sz;
	std::int16_t m_curr_idx;
};


template<typename OIt>
OIt to_utf8(std::uint32_t cp, OIt out) {
	std::optional<codepoint> ocp = codepoint::to_codepoint(cp);
	if (!ocp) {
		return out;
	}
	utf8_generator g(*ocp);
	out = g.get_all(out);
	return out;
}


// For a given codepoint, generates in order the bytes in the utf16 representation
class utf16_generator {
public:
	utf16_generator()=delete;
	utf16_generator(codepoint) noexcept;

	bool is_finished() const noexcept;
	std::uint16_t get() const noexcept;
	bool go_next() noexcept;
	void reset() noexcept;

	// Unrolls the loop that the user would normally have to write
	// TODO:  Constrain
	template<typename OIt>
	OIt get_all(OIt out) const {
		if (m_sz == 1) {
			*out++ = m_u16[0];
		} else { // if (m_sz == 2)
			*out++ = m_u16[0];
			*out++ = m_u16[1];
		}
		return out;
	}

private:
	std::array<std::uint16_t,2> m_u16;
	std::int16_t m_sz;
	std::int16_t m_curr_idx;
};


// Undefined if cp is not a valid codepoint
template<typename OIt>
OIt to_utf16(std::uint32_t cp, OIt out) {
	std::optional<codepoint> ocp = codepoint::to_codepoint(cp);
	if (!ocp) {
		return out;
	}
	utf16_generator g(*ocp);
	out = g.get_all(out);
	return out;
}


