#include "utflib/utflib.h"
#include "utflib/iterators.h"
#include "utflib/low_level.h"
#include "utflib/encoders.h"
#include "utils.h"
#include <random>
#include <filesystem>
#include <iostream>
#include <format>

void print_random_utf8_sequences() {
	std::random_device r;
	std::default_random_engine re(r());
	int n_sequences = 10;
	int n_cp = 10;
	std::string s;

	for (int i=0; i<n_sequences; ++i) {
		std::vector<std::uint8_t> v;
		auto it = std::back_inserter(v);
		it = random_utf8_sequence({0.0,1.0,0.0,0.0}, n_cp, re, it);
		for (const std::uint8_t b : v) {
			s += std::format("{:#04X}, ", b);
		}
		std::replace(s.begin(),s.end(),'X','x');
		s += std::format("\n");
		v.clear();
	}
	std::cout << s << std::endl;
}

void print_random_utf16_sequences() {
	std::random_device r;
	std::default_random_engine re(r());
	int n_sequences = 20;
	int n_cp = 10;
	std::string s;

	for (int i=0; i<n_sequences; ++i) {
		std::vector<std::uint16_t> v;
		auto it = std::back_inserter(v);
		it = random_utf16_sequence({0.5,0.5}, n_cp, re, it);
		for (const std::uint16_t w : v) {
			s += std::format("{:#04X}, ", w);
		}
		std::replace(s.begin(),s.end(),'X','x');
		s += std::format("\n");
		v.clear();
	}
	std::cout << s << std::endl;
}


void random_utf8_to_and_from_utf32() {
	std::random_device r;
	std::default_random_engine re(r());
	int n_sequences = 10000;
	int n_cp = 1;

	for (int i=0; i<n_sequences; ++i) {
		std::vector<std::uint8_t> random_utf8;
		auto it = std::back_inserter(random_utf8);
		it = random_utf8_sequence({0.25,0.25,0.25,0.25}, n_cp, re, it);
		expect(is_valid_utf8_single_codepoint(random_utf8));

		std::uint32_t random_utf32 = to_utf32(random_utf8);
		expect(is_valid_cp(random_utf32));

		std::vector<std::uint8_t> utf8_from_utf32;
		auto it2 = std::back_inserter(utf8_from_utf32);
		it2 = to_utf8(random_utf32,it2);

		expect(random_utf8.size() == utf8_from_utf32.size());
		for (int i=0; i<random_utf8.size(); ++i) {
			expect(random_utf8[i]==utf8_from_utf32[i]);
		}
	}
}

void random_utf16_to_and_from_utf32() {
	std::random_device r;
	std::default_random_engine re(r());
	int n_sequences = 10000;
	int n_cp = 1000;  // num codepoints per sequence

	for (int i=0; i<n_sequences; ++i) {
		std::vector<std::uint16_t> random_utf16;
		{
			auto it = std::back_inserter(random_utf16);
			it = random_utf16_sequence({0.5,0.5}, n_cp, re, it);
		}

		std::vector<std::uint32_t> random_utf32_from_random_utf16;
		utf16_iterator u16it(random_utf16);
		while (!u16it.is_finished()) {
			std::optional<codepoint> ocp = u16it.get_codepoint();
			expect(ocp.has_value());
			std::int32_t cp = ocp->get();
			random_utf32_from_random_utf16.push_back(cp);
			u16it.go_next();
		}
		expect(random_utf32_from_random_utf16.size()==n_cp);

		std::vector<std::uint16_t> random_utf16_from_random_utf32;
		auto it = std::back_inserter(random_utf16_from_random_utf32);
		for (const std::uint32_t cp : random_utf32_from_random_utf16) {
			it = to_utf16(cp, it);
		}

		// Validate that the back-calculated utf16 is what was generated initially
		expect(random_utf16.size() == random_utf16_from_random_utf32.size());
		for (int j=0; j<random_utf16.size(); ++j) {
			expect(random_utf16[j] == random_utf16_from_random_utf32[j]);
		}
	}
}

void utf8_iterate_sample() {
	//std::vector<std::uint8_t> u8data   {0xF0, 0x9F, 0x99, 0x83, 0x61, 0xED, 0xEE, 0xF0, 0xF0, 0x9F, 0x98, 0x80, 0x64, 0x65, 0x66};
	//std::vector<std::uint32_t> u32data {0x1F643,                0x61, 0xFFFD,           0x1F600,                0x64, 0x65, 0x66};
	std::vector<std::uint8_t> u8data   {0xF0, 0x9F, 0x99, 0x93};
	std::vector<std::uint32_t> u32data {0xFFFD,               };


	utf8_iterator it(u8data);
	std::size_t idx_u32 {0};
	while (!it.is_finished()) {
		std::optional<codepoint> ocp = it.get_codepoint();
		if (ocp) {
			bool b = ocp->get() == u32data[idx_u32];
		} else {
			bool b = u32data[idx_u32] == 0xFFFD;
		}
		it.go_next();
		++idx_u32;
	}
}

void utf16_iterate_sample() {
	std::vector<std::uint16_t> u16 {0xdba3u, 0xdd63u};

	std::vector<std::uint32_t> u32;
	utf16_iterator u16it(u16);
	while (!u16it.is_finished()) {
		std::optional<codepoint> ocp = u16it.get_codepoint();
		expect(ocp.has_value());
		std::int32_t cp = ocp->get();
		u32.push_back(cp);
		u16it.go_next();
	}

	return;
}

bool random_utf16_file(const std::filesystem::path fp, int nlns, int ncp_ln) {
	std::random_device r;
	std::default_random_engine re(r());

	std::vector<std::uint16_t> u16;
	for (int i=0; i<nlns; ++i) {
		auto it = std::back_inserter(u16);
		it = random_utf16_sequence({0.5,0.5}, ncp_ln, re, it);
		*it = std::uint16_t {0xAu};
	}

	const std::byte* p = reinterpret_cast<const std::byte*>(u16.data());
	std::size_t nbytes = (sizeof(std::uint16_t)/sizeof(std::byte))*u16.size();
	return writefile(fp, {p, nbytes});
}

void random_utf16_wch2multib();


int main(int argc, char* argv[]) {
	/*if (argc != 2) {
		std::cout << "Expected a file path" << std::endl;
		return 1;
	}*/
	
	random_utf16_wch2multib();
	//utf16_iterate_sample();
	//random_utf16_to_and_from_utf32();
	//print_random_utf8_sequences();
	//print_random_utf16_sequences();
	//random_to_and_from_utf32();
	//iterate_sample();
	//random_utf16_file("D:\\dev\\utflib\\ru16.txt", 1, 10);
	

	/*std::filesystem::path file{argv[1]};
	std::vector<std::byte> file_data = readfile(file);
	std::span<const char> s {reinterpret_cast<const char*>(file_data.data()), file_data.size()};
	bool b = utfchk(s);*/

	//f();

	return 0;
}
