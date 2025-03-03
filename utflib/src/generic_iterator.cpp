#include "utflib/generic_iterator.h"

#include "utflib/byte_manip.h"
#include "utflib/low_level.h"
#include <span>
#include <cstdint>
#include <optional>


std::optional<int> utf8_customizer::pred(std::span<const utf8_customizer::underlying> s) {
	return begins_with_valid_utf8(s);
}

std::optional<int> utf16_customizer::pred(std::span<const utf16_customizer::underlying> s) {
	return begins_with_valid_utf16(s);
}
