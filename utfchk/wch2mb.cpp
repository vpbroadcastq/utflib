#include "utflib/utflib.h"
#include "utflib/iterators.h"
#include "utflib/low_level.h"
#include "utflib/encoders.h"
#include "utils.h"
#include <random>
#include <filesystem>
#include <iostream>
#include <format>

#include <windows.h>
#include <stringapiset.h>


void random_utf16_wch2multib() {
	std::random_device r;
	std::default_random_engine re(r());
	int n_sequences = 10000;
	int n_cp = 100;  // num codepoints per sequence

	for (int i=0; i<n_sequences; ++i) {
		std::vector<std::uint16_t> random_utf16;
		{
			auto it = std::back_inserter(random_utf16);
			it = random_utf16_sequence({1.00,0.00}, n_cp, re, it);
		}

		std::vector<std::uint32_t> utf32_from_random_utf16;
		{
			utf16_iterator u16it(random_utf16);
			while (!u16it.is_finished()) {
				std::optional<codepoint> ocp = u16it.get_codepoint();
				expect(ocp.has_value());
				std::int32_t cp = ocp->get();
				utf32_from_random_utf16.push_back(cp);
				u16it.go_next();
			}
			expect(utf32_from_random_utf16.size()==n_cp);
		}

		std::vector<std::uint8_t> utf8_from_utf32;
		{
			auto it = std::back_inserter(utf8_from_utf32);
			for (const std::uint32_t cp : utf32_from_random_utf16) {
				it = to_utf16(cp, it);
			}
		}

		// wch2mb
		std::vector<std::uint8_t> utf8_from_wch2mb(utf8_from_utf32.size()*2,0u);
		// UTF-8 => lpDefaultChar and lpUsedDefaultChar must be set to NULL.
		int wch2mb = WideCharToMultiByte(CP_UTF8,
			WC_SEPCHARS,
			reinterpret_cast<const wchar_t*>(random_utf16.data()),
			random_utf16.size(),  // # of _characters_ - not clear what this means; could null-terminate to avoid
			reinterpret_cast<char*>(utf8_from_wch2mb.data()),
			utf8_from_wch2mb.size(),  // cbMultiByte
			nullptr,
			nullptr);

		if (wch2mb==0) {
			auto err = GetLastError();
			if (err == ERROR_INSUFFICIENT_BUFFER) {
				std::cout << "ERROR_INSUFFICIENT_BUFFER\n";
			} else if (err == ERROR_INVALID_FLAGS) {
				std::cout << "ERROR_INVALID_FLAGS\n";
			} else if (err == ERROR_INVALID_PARAMETER) {
				std::cout << "ERROR_INVALID_PARAMETER\n";
			} else if (err == ERROR_NO_UNICODE_TRANSLATION) {
				std::cout << "ERROR_NO_UNICODE_TRANSLATION\n";
			}
			std::cout << std::endl;
		}

		expect(static_cast<std::size_t>(wch2mb)==utf8_from_wch2mb.size());

		//validate equal
	}
}






