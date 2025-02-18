#pragma once
#include <cstddef>
#include <cstdint>


// TODO:  Constrain?
template<typename T>
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
