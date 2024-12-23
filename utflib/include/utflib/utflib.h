#pragma once
#include <cstdint>
#include <span>
#include <filesystem>
#include <vector>

// char (tho signdness is ambiguous) instead of:
// -uint8_t... maybe it should be uint8_t
// -char8_t because the data is only putatively utf8
// -std::byte because std::byte is a pain in the ass
bool utfchk(std::span<const char>);
void expect(bool, const char* = nullptr);


class utf8_error {
public:
	enum class error : std::uint8_t {
		invalid_initial_byte,
		invalid_secondary_byte
	};

	// You can't make a utf8_error unless there is actually an error
	// Note that is is possible to choose inconsistent combinations of values, however
	utf8_error()=delete;
	utf8_error(utf8_error::error, std::uint32_t, std::uint8_t) noexcept;

	// The index of the bad byte within the subsequence.  [0,3].  
	int idx_bad_byte() const noexcept;
	std::uint8_t bad_byte() const noexcept;

	// The anticipated size of the sequence computed from the initial byte
	// What if it's the initial byte that is bad?
	int sequence_size() const noexcept;

	// The idx requested must be <= idx_bad_byte().
	std::uint8_t byte(int) const noexcept;

private:
	std::uint32_t m_in_progress_cp;
	utf8_error::error m_error;
	std::uint8_t m_bad_byte;
};


class maybe_codepoint {
public:
	// The result is undefined if you call either getter on an inappropriate object
	std::uint32_t get_codepoint() const noexcept;
	utf8_error get_error() const noexcept;

	operator bool() const noexcept;
private:
	union u {
		utf8_error err;
		std::uint32_t cp;
	};
	u m_data;
	bool m_has_error;
};


class utf8_iterator {
public:
	utf8_iterator()=delete;
	explicit utf8_iterator(std::span<const char>) noexcept;

	maybe_codepoint operator*() const noexcept;
	void operator++() noexcept;
	void operator++(int) noexcept;
private:
	const char* m_pbeg;
	const char* m_pend;
	const char* m_p;
};

int f();
