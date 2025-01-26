#include "utflib/iterators.h"

#include "utflib/low_level.h"
#include <span>
#include <cstdint>
#include <optional>


utf8_iterator::utf8_iterator(std::span<const std::uint8_t> s) {
	m_pbeg = s.data();
	m_p = s.data();
	m_pend = s.data() + s.size();
}

bool utf8_iterator::go_next() {
	if (m_p == m_pend) {
		return false;
	}
	std::span<const std::uint8_t> first_valid = seek_to_first_valid_utf8_sequence({m_p,static_cast<std::size_t>(m_pend-m_p)});
	
	if (m_p == first_valid.data()) {
		// The current position (m_p) is the start of a valid byte sequence.
		m_p = first_valid.data() + first_valid.size();
	} else {
		// The current position (m_p) is the start of an invalid byte sequence, and seek_to_first_valid_utf8_sequence()
		// skipped over it.
		m_p = const_cast<std::uint8_t*>(first_valid.data());
	}
	return true;
}

bool utf8_iterator::go_prev() {
	if (m_p == m_pbeg) {
		return false;
	}
	// It won't work to seek backward until the first byte for which seek_to_first_valid_utf8_sequence() != m_p,
	// because you could have a sequence like this: [valid utf8 seq] [error seq] [valid utf8 seq], with
	// m_p at the start of the second utf8 seq.
	// While traversing backwards through the error seq and the first utf8 seq, seek_to_first_valid_utf8_sequence()
	// will return the second utf8 seq until the iterator reaches the first byte of the first utf8 seq.
	// However, go_prev() needs to position the iterator on the first byte of the error seq to match the
	// reverse of the go_next() behavior.
	const std::uint8_t* p = m_p;
	std::span<const std::uint8_t> prev_valid;
	while (true) {
		--p;
		if (p == m_pbeg) {
			// May or may not be the start of an error seq
			m_p = p;
			return true;
		}
		prev_valid = seek_to_first_valid_utf8_sequence({p,static_cast<std::size_t>(m_p-p)});
		if (prev_valid.data() != m_p) {
			break;
		}
	}
	// prev_valid.data() != m_p
	// The question now is:  Is there an error seq between pre_valid.data() and m_p?
	if (prev_valid.data() + prev_valid.size() == m_p) {
		// Nope
		m_p = prev_valid.data();
	} else {
		m_p = prev_valid.data() + prev_valid.size();
	}
	return true;
}

bool utf8_iterator::is_finished() const {
	return m_p == m_pend;
}

bool utf8_iterator::at_start() const {
	return m_p == m_pbeg;
}

std::optional<codepoint> utf8_iterator::get_codepoint() const {
	std::span<const std::uint8_t> next_valid = seek_to_first_valid_utf8_sequence({m_p,static_cast<std::size_t>(m_pend-m_p)});
	if (m_p != next_valid.data() || next_valid.size()==0) {
		return std::nullopt;
	}
	return codepoint(next_valid);
}

std::optional<utf8_codepoint> utf8_iterator::get_utf8() const {
	std::span<const std::uint8_t> next_valid = seek_to_first_valid_utf8_sequence({m_p,static_cast<std::size_t>(m_pend-m_p)});
	if (m_p != next_valid.data() || next_valid.size()==0) {
		return std::nullopt;
	}
	return utf8_codepoint(next_valid);
}

std::span<const std::uint8_t> utf8_iterator::get_underlying() const {
	std::span<const std::uint8_t> next_valid = seek_to_first_valid_utf8_sequence({m_p,static_cast<std::size_t>(m_pend-m_p)});
	if (m_p != next_valid.data()) {
		// m_p is on the first byte of an invalid subsequence
		return {m_p,next_valid.data()};
	}
	return next_valid;
}


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
	if (m_p == m_pend) {
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
			break;  // uhh...
		}
		++curr_bytenum;
		if (curr_bytenum > sz) {
			break;  // success path
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
	if (m_p == m_pbeg) {
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
		if (!could_be_utf8_trailing_byte(*p)) {
			// could be a valid leading byte, could be a completely invalid byte like [0xC0, 0xC1], [0xF5,0xFF]
			break;
		}
	}
	// p == m_pbeg || !could_be_utf8_trailing_byte(*p)
	utf8_iterator_alt it({p, m_p});
	std::span<const std::uint8_t> s;
	while (true) {
		p = it.get_underlying().data();
		it.go_next();
		s = it.get_underlying();
		if (s.data()/*+s.size() */== m_p) {
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

std::optional<utf8_codepoint> utf8_iterator_alt::get_utf8() const {
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
// utf16_iterator
//
utf16_iterator::utf16_iterator(std::span<const std::uint16_t> s) {
	m_p = s.data();
	m_pbeg = s.data();
	m_pend = s.data() + s.size();
}

bool utf16_iterator::is_finished() const {
	return m_p == m_pend;
}

bool utf16_iterator::at_start() const {
	return m_p == m_pbeg;
}

// false if it didn't go anywhere (=>is_finished() prior to the call)
bool utf16_iterator::go_next() {
	// m_p is pointing at the first word of a valid code unit sequence, the first word of an
	// invalid code unit sequence, or at the end
	if (m_p == m_pend) {
		return false;
	}
	std::span<const std::uint16_t> s = seek_to_first_valid_utf16_sequence({m_p,m_pend});
	if (s.data()!=m_p) {
		// The iterator is on the first word of an invalid sequence; s.data() is the start of the
		// next valid sequence (or the end).
		m_p = s.data();
	} else {
		// s.data() == m_p means that the iterator is on the first word of a valid sequence; simply
		// skip over it
		m_p += s.size();
	}

	return true;
}


// false if it didn't go anywhere (=>at_start() prior to the call)
bool utf16_iterator::go_prev() {
	if (m_p == m_pbeg) {
		return false;
	}

	// Seek backwards to the next valid codepoint and then advance past it
	const std::uint16_t* p = m_p;
	while (true) {
		--p;
		if (p==m_pbeg) { break; }
		std::span<const std::uint16_t> s = seek_to_first_valid_utf16_sequence({p,m_pend});
		if (s.data() == m_p) { continue; }
		// s.data()!= m_p
		
		// Below, w means a valid single-word codepoint, s s a valid 2-word codepoint, and i some
		// sort of invalid subsequence.  * indicates where p and m_p are pointing (p<m_p).
		// (a) w* i i i i w*
		// (b) s* s w*
		// (c) w* w*
		// (d) s* s i i i w*
		// For a,d need to move forward to the first I; for b,c we're done
		if (p+s.size() == m_p) {
			// Case b || c
			break;
		}
		
		// In state a || d
		if (is_valid_utf16_codepoint(*p)) {
			// a
			++p;
			break;
		} else {
			// b
			p += 2;
			break;
		}
	}
	m_p = p;

	return true;
}

std::optional<codepoint> utf16_iterator::get_codepoint() const {
	if (is_valid_utf16_codepoint(*m_p)) {
		return codepoint(utf16_to_codepoint_value(*m_p));
	}
	
	if ((m_pend-m_p)>1
		&& is_valid_utf16_surrogate_pair_leading(*m_p)
		&& is_valid_utf16_surrogate_pair_trailing(*(m_p+1))) {
		return codepoint(utf16_to_codepoint_value(*m_p, *(m_p+1)));
	}
	
	return std::nullopt;
}

std::optional<utf16_codepoint> utf16_iterator::get_utf16() const {
	std::span<const std::uint16_t> next_valid = seek_to_first_valid_utf16_sequence({m_p,m_pend});
	if (m_p != next_valid.data() || next_valid.size()==0) {
		return std::nullopt;
	}
	return utf16_codepoint(next_valid);
}
	
std::span<const std::uint16_t> utf16_iterator::get_underlying() const {
	std::span<const std::uint16_t> s = seek_to_first_valid_utf16_sequence({m_p,m_pend});
	if (m_p == s.data()) {
		// m_p is pointing at the first word of a valid code unit sequence
		return s;
	}
	// m_p is pointing at the first word of a (potentially multi-word) sequence of invalid words.
	// s.data() is pointing either at the end of the range or the start of the first valid code unit
	// sequence following the invalid sequence.
	return {m_p, s.data()};
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
	if (is_valid_utf16_codepoint(*m_p)) {
		return codepoint(utf16_to_codepoint_value(*m_p));
	}
	
	if ((m_pend-m_p)>1
		&& is_valid_utf16_surrogate_pair_leading(*m_p)
		&& is_valid_utf16_surrogate_pair_trailing(*(m_p+1))) {
		return codepoint(utf16_to_codepoint_value(*m_p, *(m_p+1)));
	}
	
	return std::nullopt;
}

std::optional<utf16_codepoint> utf16_iterator_alt::get_utf16() const {
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


