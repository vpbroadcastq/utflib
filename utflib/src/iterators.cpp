#include "utflib/iterators.h"
#include "utflib/byte_manip.h"
#include "utflib/low_level.h"
#include <span>
#include <cstdint>
#include <optional>


//
// utf8_iterator_alt
//
utf8_iterator_alt::utf8_iterator_alt(std::span<const std::uint8_t> s) {
	m_p = s.data();
	m_pbeg = s.data();
	m_pend = s.data() + s.size();
}

bool utf8_iterator_alt::is_finished() const {
	return m_p == m_pend;
}

bool utf8_iterator_alt::at_start() const {
	return m_p == m_pbeg;
}

// false if it didn't go anywhere (=>is_finished() prior to the call)
bool utf8_iterator_alt::go_next() {
	if (is_finished()) {
		return false;
	}

	// 1)  m_p could be at the start of a valid subsequence
	//     => Just compute its size and set m_p += size;
	// 2)  m_p could be at the start of an invalid subsequence with a valid leading byte
	//     => Need to compute and move past the maximal subpart.
	// 3)  m_p could be at the start of an invalid subsequence with an invalid leading byte
	//     => ++m_p; If m_p is not a valid leading byte it can't be the start of a truncated subsequence,
	//        so just move past it.  Or put another way, there is no maximal subpart.

	if (!is_valid_utf8_leading_byte(*m_p)) {
		++m_p;
		return true;
	}

	int sz = size_utf8_multibyte_seq_from_leading_byte(*m_p);
	const std::uint8_t* p = m_p;
	int curr_bytenum {1};
	while (true) {
		++p;
		if (p == m_pend) {
			break;
		}
		++curr_bytenum;
		if (curr_bytenum > sz) {
			break;
		}
		// curr_bytenum <= sz
		if (curr_bytenum == 2 && !is_valid_utf8_second_byte(*p,*m_p)) {
			break;
		}
		if ((curr_bytenum==3 || curr_bytenum==4) && !is_valid_utf8_third_or_fourth_byte(*p)) {
			break;
		}
	}
	m_p = p;
	return true;
}

// false if it didn't go anywhere (=>at_start() prior to the call)
bool utf8_iterator_alt::go_prev() {
	if (at_start()) {
		return false;
	}
	// Seek backwards to the first putative leading byte, then forwards from there to the first
	// position that ends at m_p.
	const std::uint8_t* p = m_p;
	while (true) {
		--p;
		if (p == m_pbeg) {
			break;
		}
		if (!is_utf8_trailing_byte(*p)) {
			// Could be a valid leading byte, could be a completely invalid byte like [0xC0, 0xC1], [0xF5,0xFF]
			break;
		}
	}
	// p == m_pbeg || !is_utf8_trailing_byte(*p)
	utf8_iterator_alt it({p, m_p});
	std::span<const std::uint8_t> s;
	while (true) {
		p = it.get_underlying().data();
		it.go_next();
		if (it.get_underlying().data() == m_p) {
			break;
		}
	}
	m_p = p;
	return true;
}

std::optional<codepoint> utf8_iterator_alt::get_codepoint() const {
	std::span<const std::uint8_t> next_valid = seek_to_first_valid_utf8_sequence({m_p,static_cast<std::size_t>(m_pend-m_p)});
	if (m_p != next_valid.data() || next_valid.size()==0) {
		return std::nullopt;
	}
	return codepoint(next_valid);
}

std::optional<utf8_codepoint> utf8_iterator_alt::get() const {
	std::span<const std::uint8_t> next_valid = seek_to_first_valid_utf8_sequence({m_p,static_cast<std::size_t>(m_pend-m_p)});
	if (m_p != next_valid.data() || next_valid.size()==0) {
		return std::nullopt;
	}
	return utf8_codepoint(next_valid);
}

std::span<const std::uint8_t> utf8_iterator_alt::get_underlying() const {
	utf8_iterator_alt it = *this;
	it.go_next();
	return {m_p, it.m_p};
}


//
// utf16_iterator_alt
//
utf16_iterator_alt::utf16_iterator_alt(std::span<const std::uint16_t> s) {
	m_p = s.data();
	m_pbeg = s.data();
	m_pend = s.data() + s.size();
}

bool utf16_iterator_alt::is_finished() const {
	return m_p == m_pend;
}

bool utf16_iterator_alt::at_start() const {
	return m_p == m_pbeg;
}

// false if it didn't go anywhere (=>is_finished() prior to the call)
bool utf16_iterator_alt::go_next() {
	// m_p is pointing at the first word of a valid code unit sequence, an invalid code unit,
	// or at the end.
	if (m_p == m_pend) {
		return false;
	}

	if (is_valid_utf16_codepoint(*m_p)) {
		++m_p;
	} else if ((m_pend-m_p)>=2 && is_valid_utf16_surrogate_pair(*m_p, *(m_p+1))) {
		m_p += 2;
	} else {
		// m_p is on an invalid code unit
		++m_p;
	}

	return true;
}


// false if it didn't go anywhere (=>at_start() prior to the call)
bool utf16_iterator_alt::go_prev() {
	if (m_p == m_pbeg) {
		return false;
	}

	// Seek backwards to the start of the next valid code unit sequence, the next invalid
	// code unit, or the beginning, whichever comes first.
	--m_p;
	if (m_p==m_pbeg) {
		return true;
	}
	if (is_valid_utf16_codepoint(*m_p)) {
		return true;
	}
	if (is_valid_utf16_surrogate_pair_leading(*(m_p-1))
		&& is_valid_utf16_surrogate_pair_trailing(*m_p)) {
		--m_p;
		return true;
	}
	// invalid

	return true;
}

std::optional<codepoint> utf16_iterator_alt::get_codepoint() const {
	std::span<const std::uint16_t> next_valid = seek_to_first_valid_utf16_sequence({m_p,m_pend});
	if (m_p != next_valid.data() || next_valid.data()==0) {
		return std::nullopt;
	}

	if (next_valid.size()==1) {
		return codepoint(utf16_to_codepoint_value(next_valid[0]));
	}

	return codepoint(utf16_to_codepoint_value(next_valid[0],next_valid[1]));
}

std::optional<utf16_codepoint> utf16_iterator_alt::get() const {
	std::span<const std::uint16_t> next_valid = seek_to_first_valid_utf16_sequence({m_p,m_pend});
	if (m_p != next_valid.data() || next_valid.size()==0) {
		return std::nullopt;
	}
	return utf16_codepoint(next_valid);
}

std::span<const std::uint16_t> utf16_iterator_alt::get_underlying() const {
	utf16_iterator_alt it = *this;
	it.go_next();
	return {m_p, it.m_p};
}



//
// utf32_iterator_swapping
//
utf32_iterator_swapping::utf32_iterator_swapping(std::span<const std::uint32_t> s) {
	m_p = s.data();
	m_pbeg = s.data();
	m_pend = s.data() + s.size();
}

bool utf32_iterator_swapping::is_finished() const {
	return m_p == m_pend;
}

bool utf32_iterator_swapping::at_start() const {
	return m_p == m_pbeg;
}

// false if it didn't go anywhere (=>is_finished() prior to the call)
// TODO:  Should I use is_valid_utf32_codepoint_reversed?
bool utf32_iterator_swapping::go_next() {
	// m_p is pointing at the first dw of a valid code unit sequence (of length 1 because utf-32),
	// the first dw of an invalid code unit sequence, or at the end.
	if (is_finished()) {
		return false;
	}

	if (is_valid_utf32_codepoint(reverse_bytes(*m_p))) {
		// Step over the current valid code unit onto whatever comes next (valid or invalid)
		++m_p;
	} else {
		m_p = seek_to_first_valid_utf32_sequence_reversed({m_p,m_pend}).data();
	}
	return true;
}


// false if it didn't go anywhere (=>at_start() prior to the call)
bool utf32_iterator_swapping::go_prev() {
	if (at_start()) {
		return false;
	}

	const std::uint32_t* p = m_p;
	--p;
	if (p==m_pbeg) {
		m_p = p;
		return true;
	}

	if (is_valid_utf32_codepoint(reverse_bytes(*p))) {
		m_p = p;
		return true;
	} else {
		// p is invalid and p != m_pbeg
		while (true) {
			--p;
			if (is_valid_utf32_codepoint(reverse_bytes(*p))) {
				++p;  // Need to point at the first dw of the _invalid_ seq
				m_p = p;
				return true;
			}
			if (p==m_pbeg) {
				// p is invalid
				m_p = p;
				return true;
			}
		}
	}

	return true;  // Not reached
}

std::optional<codepoint> utf32_iterator_swapping::get_codepoint() const {
	if (!is_finished() && is_valid_utf32_codepoint(reverse_bytes(*m_p))) {
		return codepoint(reverse_bytes(*m_p));
	}
	return std::nullopt;
}

std::optional<utf32_codepoint_swapped> utf32_iterator_swapping::get() const {
	if (!is_finished() && is_valid_utf32_codepoint(reverse_bytes(*m_p))) {
		return utf32_codepoint_swapped(std::span<const std::uint32_t>{m_p,m_p+1});
	}
	return std::nullopt;
}
	
std::span<const std::uint32_t> utf32_iterator_swapping::get_underlying() const {
	if (is_finished()) {
		return {m_p, m_pend};
	}

	if (is_valid_utf32_codepoint(*m_p)) {
		return {m_p, m_p+1};
	}

	// m_p is at the start of an invalid sequence
	return seek_to_first_valid_utf32_sequence_reversed({m_p, m_pend});
}


//
// utf32_iterator_alt
//
utf32_iterator_alt::utf32_iterator_alt(std::span<const std::uint32_t> s) {
	m_p = s.data();
	m_pbeg = s.data();
	m_pend = s.data() + s.size();
}

bool utf32_iterator_alt::is_finished() const {
	return m_p == m_pend;
}

bool utf32_iterator_alt::at_start() const {
	return m_p == m_pbeg;
}

// false if it didn't go anywhere (=>is_finished() prior to the call)
bool utf32_iterator_alt::go_next() {
	// m_p is pointing at the first dw of a valid code unit sequence (of length 1 because utf-32),
	// the first dw of an invalid code unit sequence, or at the end.
	if (is_finished()) {
		return false;
	}

	++m_p;

	return true;
}


// false if it didn't go anywhere (=>at_start() prior to the call)
bool utf32_iterator_alt::go_prev() {
	if (at_start()) {
		return false;
	}

	--m_p;

	return true;
}

std::optional<codepoint> utf32_iterator_alt::get_codepoint() const {
	if (!is_finished() && is_valid_utf32_codepoint(*m_p)) {
		return codepoint(*m_p);
	}
	return std::nullopt;
}

std::optional<utf32_codepoint> utf32_iterator_alt::get() const {
	if (!is_finished() && is_valid_utf32_codepoint(*m_p)) {
		return utf32_codepoint(std::span<const std::uint32_t>{m_p,m_p+1});
	}
	return std::nullopt;
}
	
std::span<const std::uint32_t> utf32_iterator_alt::get_underlying() const {
	if (is_finished()) {
		return {m_p, m_pend};
	}

	return {m_p, m_p+1};
}


//
// utf32_iterator_alt_swapping
//
utf32_iterator_alt_swapping::utf32_iterator_alt_swapping(std::span<const std::uint32_t> s) {
	m_p = s.data();
	m_pbeg = s.data();
	m_pend = s.data() + s.size();
}

bool utf32_iterator_alt_swapping::is_finished() const {
	return m_p == m_pend;
}

bool utf32_iterator_alt_swapping::at_start() const {
	return m_p == m_pbeg;
}

// false if it didn't go anywhere (=>is_finished() prior to the call)
bool utf32_iterator_alt_swapping::go_next() {
	// m_p is pointing at the first dw of a valid code unit sequence (of length 1 because utf-32),
	// the first dw of an invalid code unit sequence, or at the end.
	if (is_finished()) {
		return false;
	}

	++m_p;

	return true;
}

// false if it didn't go anywhere (=>at_start() prior to the call)
bool utf32_iterator_alt_swapping::go_prev() {
	if (at_start()) {
		return false;
	}

	--m_p;

	return true;
}

std::optional<codepoint> utf32_iterator_alt_swapping::get_codepoint() const {
	if (!is_finished() && is_valid_utf32_codepoint(reverse_bytes(*m_p))) {
		return codepoint(reverse_bytes(*m_p));
	}
	return std::nullopt;
}

std::optional<utf32_codepoint_swapped> utf32_iterator_alt_swapping::get() const {
	if (!is_finished() && is_valid_utf32_codepoint(reverse_bytes(*m_p))) {
		return utf32_codepoint_swapped(std::span<const std::uint32_t>{m_p,m_p+1});
	}
	return std::nullopt;
}
	
std::span<const std::uint32_t> utf32_iterator_alt_swapping::get_underlying() const {
	if (is_finished()) {
		return {m_p, m_pend};
	}

	return {m_p, m_p+1};
}

