#include "utflib/utflib.h"

#include <span>
#include <cstdint>

void expect(bool b, const char* msg) {
	if (b) { return; }
	std::abort();
}

bool is_valid_initial_byte(std::uint8_t b) {
	return (b>>7)==0 || (b>>5)==0b1110 || (b>>4)==0b1110 || (b>>3)==0b11110;
}

bool is_valid_secondary_byte(std::uint8_t b) {
	return (b>>6)==0b10;
}

// From the first byte of a multibyte sequence, computes the number of bytes
// in the sequence.
// Result is undefined if c is not a valid initial byte of a multibyte sequence
int size_multibyte(std::uint8_t c) {
	if ((c & 0b10000000) == 0) {
		return 1;
	} else if ((c & 0b11100000) == 0b11000000) {
		return 2;
	} else if ((c & 0b11110000) == 0b11100000) {
		return 3;
	} else {
		return 4;
	}
}

// Extracts the value bits from the initial byte of a potentially multibyte sequence.
// sz_multib == size_multibyte(b); the result is undefined if sz_multib is incorrect
// or if b is not a valid initial byte (ie, if !is_valid_initial_byte(b)).
std::uint8_t payload_initial_byte(std::uint8_t b, int sz_multib) {
	if (sz_multib == 1) {
		return 0x7Fu & b;
	} else if (sz_multib == 2) {
		return 0x1Fu & b;
	} else if (sz_multib == 3) {
		return 0x0Fu & b;
	} else { // (sz_multib == 4) 
		return 0x07u & b;
	}
}

// Extracts the value bits from a secondary byte of a multibyte sequence.  The result
// is undefined if !is_valid_secondary_byte(b).
std::uint8_t payload_secondary_byte(std::uint8_t b) {
	return 0x3Fu & b;
}

// Why do i need to say 'const' char?  Aren't spans immutable?
bool utfchk(std::span<const char> data) {
	int idx = 0;
	int idx_mb = 0; // current index within multibyte code point
	int sz_curr_mb = 0;  // the expected size of the current multibyte code point
	uint32_t curr_codepoint = 0;
	for (const auto e : data) {
		const std::uint8_t c = static_cast<std::uint8_t>(e);
		if (idx_mb == 0) {
			expect(sz_curr_mb == 0);
			if (!is_valid_initial_byte(c)) {
				return false;
			}
			sz_curr_mb = size_multibyte(c);
			curr_codepoint = payload_initial_byte(c, sz_curr_mb);
			++idx_mb;
		} else if (idx_mb == 1) {  // 1, 2, and 3 are all the same?
			expect(sz_curr_mb >= 1);
			if (!is_valid_secondary_byte(c)) {
				return false;
			}
			curr_codepoint <<= 6;
			curr_codepoint += payload_secondary_byte(c);
			++idx_mb;
		} else if (idx_mb == 2) {
			expect(sz_curr_mb >= 2);
			if (!is_valid_secondary_byte(c)) {
				return false;
			}
			curr_codepoint <<= 6;
			curr_codepoint += payload_secondary_byte(c);
			++idx_mb;
		} else if (idx_mb == 3) {
			expect(sz_curr_mb >= 3);
			if (!is_valid_secondary_byte(c)) {
				return false;
			}
			curr_codepoint <<= 6;
			curr_codepoint += payload_secondary_byte(c);
			++idx_mb;
		}

		// Finished with the current codepoint?
		if (sz_curr_mb == idx_mb) {
			sz_curr_mb = 0;
			idx_mb = 0;
			curr_codepoint = 0;
		}

		++idx;
	}

	return true;
}


utf8_iterator::utf8_iterator(std::span<const char> s) noexcept {
	m_pbeg = s.data();
	m_pend = s.data()+s.size();
	m_p = m_pbeg;
}

