#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <concepts>


// Clang turns this in to a single bswap; GCC and MSVC don't.  C++23 has std::byteswap.
template<std::integral T>
constexpr T reverse_bytes(const T val) {
	if constexpr (sizeof(T) == 0) {
		return val;
	}

	T result;
	std::byte* dest = reinterpret_cast<std::byte*>(&result) + sizeof(T);
	--dest;
	const std::byte* p = reinterpret_cast<const std::byte*>(&val);
	const std::byte* p_end = reinterpret_cast<const std::byte*>(&val) + sizeof(T);
	while (p != p_end) {
		*dest = *p;
		++p;
		--dest;
	}
	return result;
}



