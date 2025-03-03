#pragma once
#include "utflib.h"
#include <cstdint>
#include <span>
#include <iterator>
#include <optional>

// Alternatives:  Could have a data member that holds the last size, valid if the predicate returned true.
// Might be better, because this thing could also return the length of an invalid subseq if that's what the
// predicate gets fed.
// The only members of this cusotmization type should be things that are actually customizable.  Since the
// use of span is not, span shouldn't be mentioned here.  Since the pointer type has to comport with the
// underlying type, there shouldn't be a seperate entry for underlying_ptr, etc.
struct utf8_customizer {
	using underlying = std::uint8_t;
	using codepoint_type = utf8_codepoint;
	static std::optional<int> pred(std::span<const underlying>);
};

struct utf16_customizer {
	using underlying = std::uint16_t;
	using codepoint_type = utf16_codepoint;
	static std::optional<int> pred(std::span<const underlying>);
};

template<typename custom>
class utf_iterator {
public:
	using difference_type = std::ptrdiff_t;
    using value_type = std::optional<codepoint>;
	utf_iterator()=default;
	explicit utf_iterator(std::span<const typename custom::underlying> s) {
		m_pbeg = s.data();
		m_p = s.data();
		m_pend = s.data() + s.size();
	}

	bool is_finished() const {
		return m_p == m_pend;
	}
	bool at_start() const {
		return m_p == m_pbeg;
	}

	// false if it didn't go anywhere (=>is_finished() prior to the call)
	bool go_next() {
		if (is_finished()) {
			return false;
		}

		std::optional<int> sz = custom::pred({m_p,m_pend});
		if (sz) {
			m_p += *sz;
			return true;
		}

		// On the start of an invalid subsequence
		const custom::underlying* p = m_p;
		while (true) {
			++p;
			if (p == m_pend) {
				break;
			}
			sz = custom::pred({p,m_pend});
			if (sz) {
				break;
			}
		}

		m_p = p;
		return true;
	}

	// false if it didn't go anywhere (=>at_start() prior to the call)
	bool go_prev() {
		if (at_start()) {
			return false;
		}

		const custom::underlying* p = m_p;
		std::optional<int> sz = std::nullopt;
		while (true) {
			--p;
			sz = custom::pred({p,m_pend});
			if (p==m_pbeg || sz) {
				break;
			}
		}
		// p does not need to move backwards any further.  Either p is on a valid start byte or p is at the beginning
		// of the sequence.  In either event, p may need to move forward.
		if (sz) {
			if (p+*sz == m_p) {
				// p is on a valid start byte and there is no intervening invalid sequence between the valid sequence
				// beginning at p and the valid sequence beginning at m_p.
			} else {
				// p is on a valid start byte but p + *sz > m_p.  There is an invalid byte sequence between the valid
				// byte sequence starting at p and the valid byte sequence starting at m_p.  p needs to be moved forward
				// to the beginning of the invalid byte sequence.
				p += *sz;
			}
		} else {  // !sz
			// p is on an at the very beginning of the sequence; the sequence begins with an invalid byte sequence.
		}

		m_p = p;
		return true;
	}

	std::optional<codepoint> get_codepoint() const {
		std::optional<int> sz = custom::pred({m_p,m_pend});
		if (sz) {
			return codepoint(std::span<const custom::underlying>{m_p,m_p+*sz});
		}
		return std::nullopt;
	}
	
	std::optional<typename custom::codepoint_type> get() const {
		std::optional<int> sz = custom::pred(std::span<const custom::underlying>{m_p,m_pend});
		if (sz) {
			return custom::codepoint_type(std::span<const custom::underlying>{m_p,m_p+*sz});
		}
		return std::nullopt;
	}
	
	// This is the only getter the iterator "should" expose but since it has to compute the valid
	// code unit subsequence anyway it is effecient for it to also offer get().
	std::span<const typename custom::underlying> get_underlying() const {
		if (is_finished()) {
			return {};
		}
		std::optional<int> sz = custom::pred({m_p,m_pend});
		if (sz) {
			return {m_p,m_p+*sz};
		}

		const custom::underlying* p = m_p;
		while (true) {
			++p;
			if (p==m_pend) {
				break;
			}
			sz = custom::pred({p,m_pend});
			if (sz) {
				break;
			}
		}
		return {m_p,p};
	}

	// The standard C++ iterator operations for use with a sentinel indicating the end of the range.
	// TODO: Should these ++ & -- operations carry the iterator beyond its range?  No, obviously?
	std::optional<codepoint> operator*() const {
		return get_codepoint();
	}

	utf_iterator& operator++() {
		go_next();
		return *this;
	}

	utf_iterator operator++(int) {
		utf_iterator copy = *this;
		go_next();
		return copy;
	}

	utf_iterator& operator--() {
		go_prev();
		return *this;
	}

	utf_iterator operator--(int) {
		utf_iterator copy = *this;
		go_prev();
		return copy;
	}

	// TODO:  Note how I'm only comparing all three pointers and not just m_p... correct?
	// TODO: No:  Should only compare m_p
	bool operator==(const utf_iterator& lhs) const {
		return m_pbeg==lhs.m_pbeg && m_p==lhs.m_p && m_pend==lhs.m_pend;
	}
private:
	const custom::underlying* m_p {};
	const custom::underlying* m_pbeg {};
	const custom::underlying* m_pend {};
};


