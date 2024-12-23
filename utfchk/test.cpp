#include <vector>
#include <cstdint>
#include "utflib/utflib.h"


struct test_data {
	std::vector<std::uint8_t> d;
	std::vector<std::uint32_t> codepoints;
	std::size_t idx_init_invalid;
	bool is_valid;
};

std::vector<test_data> g_td_valid {
	{
		.d = {0x73, 0x74, 0x64, 0x3A, 0x3A, 0x66, 0x72, 0x65, 0x61, 0x64, 0x0D, 0x0A,
			0x20, 0x43, 0x2B, 0x2B},
		.codepoints = {0x73, 0x74, 0x64, 0x3A, 0x3A, 0x66, 0x72, 0x65, 0x61, 0x64, 0x0D, 0x0A,
			0x20, 0x43, 0x2B, 0x2B},
		.idx_init_invalid = 0,
		.is_valid = true
	},
	{	.d = {0x20, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x2F, 0x6F, 0x75, 0x74, 0x70, 0x75,
			0x74, 0x20, 0x6C, 0x69},
		.codepoints = {0x20, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x2F, 0x6F, 0x75, 0x74, 0x70, 0x75,
			0x74, 0x20, 0x6C, 0x69},
		.idx_init_invalid = 0,
		.is_valid = true
	},
	{	.d = {0x3C, 0x20, 0x31, 0x20, 0x3C, 0x3C, 0x20, 0x27, 0x20, 0x27, 0x20, 0x3C,
			0x3C, 0x20, 0x32, 0x20},
		.codepoints = {0x3C, 0x20, 0x31, 0x20, 0x3C, 0x3C, 0x20, 0x27, 0x20, 0x27, 0x20, 0x3C,
			0x3C, 0x20, 0x32, 0x20},
		.idx_init_invalid = 0,
		.is_valid = true
	},
};

std::vector<test_data> g_td_invalid {
	{
		.d = {0xC3, 0x28},
		.codepoints = {},  // C3 is an invalid initial byte
		.idx_init_invalid = 0,
		.is_valid = false
	},
	{
		.d = {0x0D, 0x0A, 0xC3, 0x28},
		.codepoints = {0x0D, 0x0A},  // C3 is an invalid initial byte
		.idx_init_invalid = 2,
		.is_valid = false
	},
	{
		.d = {0xE2, 0x28, 0xA1},  // E2=>3 byte seq but 28 is an invalid secondary byte
		.codepoints = {},
		.idx_init_invalid = 1,
		.is_valid = false
	},
	{
		.d = {0xE2, 0xA1, 0x28},  // E2=>3 byte seq but 28 is an invalid secondary byte
		.codepoints = {},
		.idx_init_invalid = 2,
		.is_valid = false
	},
	{
		.d = {0xF0, 0x28, 0x8C, 0xBC},  // F0=>4 byte seq but 28 is an invalid secondary byte
		.codepoints = {},
		.idx_init_invalid = 1,
		.is_valid = false
	},
	{
		.d = {0xF0, 0x8C, 0xBC, 0x28},  // F0=>4 byte seq but 28 is an invalid secondary byte
		.codepoints = {},
		.idx_init_invalid = 3,
		.is_valid = false
	},
	{
		.d = {0x0D, 0x0A, 0xF0, 0x8C, 0xBC, 0x28},  // F0=>4 byte seq but 28 is an invalid secondary byte
		.codepoints = {0x0D, 0x0A},
		.idx_init_invalid = 5,
		.is_valid = false
	},
};


bool test_utfchk() {
	// Valid
	for (const auto& e : g_td_valid) {
		const char* p = reinterpret_cast<const char*>(e.d.data());
		bool b = utfchk({p, e.d.size()});
		if (!b) {
			std::abort();
		}
	}

	// Invalid
	for (const auto& e : g_td_invalid) {
		const char* p = reinterpret_cast<const char*>(e.d.data());
		bool b = utfchk({p, e.d.size()});
		if (b) {
			std::abort();
		}
	}
}

