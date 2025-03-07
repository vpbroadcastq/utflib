#include "utflib/utflib.h"
#include "utflib/byte_manip.h"
#include "utflib/iterators.h"
#include "utflib/low_level.h"
#include "utflib/encoders.h"
#include "utils.h"
#include <random>
#include <filesystem>
#include <iostream>
#include <format>
#include <string>
#include <algorithm>
#include <limits>


std::string codepoints_bytes_reversed() {
	std::string s;
	std::vector<std::uint32_t> v;
	for (std::uint32_t i=0; i<std::numeric_limits<std::uint32_t>::max(); ++i) {
		v.push_back(reverse_bytes(i));
	}
	std::ranges::sort(v);

	bool was_last_valid {false};
	for (std::uint32_t i=0; i<std::numeric_limits<std::uint32_t>::max(); ++i) {
		std::uint32_t i_rev = v[i];//reverse_bytes(i);
		bool is_valid = is_valid_utf32_codepoint_reversed(i_rev);
		if (is_valid && !was_last_valid) {
			s += std::to_string(i_rev);
			s += " VALID... \n";
		} else if (!is_valid && was_last_valid) {
			s += std::to_string(i_rev);
			s += " INVALID... \n";
		}
		was_last_valid = is_valid;
	}

	return s;
}

std::string fwd_vs_reverse() {
	std::string s;
	for (std::uint32_t i=0; i<1000; ++i) {
		s += std::format("{}, {}\n", i, reverse_bytes(i));
	}
	return s;
}


std::string get_random_utf8_sequences() {
	std::random_device r;
	std::default_random_engine re(r());
	int n_sequences = 1;
	int n_cp = 1024;
	std::string s;

	for (int i=0; i<n_sequences; ++i) {
		std::vector<std::uint8_t> v;
		{
			// Generate
			auto it = std::back_inserter(v);
			it = random_utf8_sequence({0.25,0.25,0.25,0.25}, n_cp, re, it);

			// Validate
			utf8_iterator u8it(v);
			while (!u8it.is_finished()) {
				expect(u8it.get_codepoint().has_value());
				u8it.go_next();
			}
		}
		for (const std::uint8_t b : v) {
			s += std::format("{:#04X}, ", b);
		}
		std::replace(s.begin(),s.end(),'X','x');
		s += std::format("\n");
		v.clear();
	}
	return s;
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
	int n_sequences = 1;
	int n_cp = 10000;

	for (int i=0; i<n_sequences; ++i) {
		std::vector<std::uint8_t> ru8;
		{
			auto it = std::back_inserter(ru8);
			it = random_utf8_sequence({0.25,0.25,0.25,0.25}, n_cp, re, it);
		}

		std::vector<std::uint32_t> ru32;
		ru32.reserve(n_cp);
		{
			utf8_iterator u8it(ru8);
			while (!u8it.is_finished()) {
				expect(u8it.get_codepoint().has_value());
				std::uint32_t cp = u8it.get_codepoint().value().get();
				expect(is_valid_cp(cp));
				ru32.push_back(cp);
				u8it.go_next();
			}
		}
		
		std::vector<std::uint8_t> ru8_back;
		auto ru8_back_it = std::back_inserter(ru8_back);
		for (const std::uint32_t curr_u32 : ru32) {
			ru8_back_it = to_utf8(curr_u32,ru8_back_it);
		}
		expect(ru8_back.size()==ru8.size());
		for (std::size_t i=0; i<ru8.size(); ++i) {
			expect(ru8[i]==ru8_back[i]);
		}
	}
	return;
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

std::vector<std::uint32_t> random_utf32_to_file(const std::filesystem::path fp, int nlns, int ncp_ln) {
	std::random_device r;
	std::default_random_engine re(r());

	std::vector<std::uint32_t> ru32;
	for (int i=0; i<nlns; ++i) {
		auto it = std::back_inserter(ru32);
		it = random_utf32_sequence(ncp_ln, re, it);
		*it = std::uint32_t {0x0Au};
	}

	const std::byte* p = reinterpret_cast<const std::byte*>(ru32.data());
	std::size_t nbytes = (sizeof(std::uint32_t)/sizeof(std::byte))*ru32.size();
	writefile(fp, {p, nbytes});
	return ru32;
}

struct random_all_encodings {
	std::vector<std::uint32_t> u32;
	std::vector<std::uint16_t> u16;
	std::vector<std::uint8_t> u8;
};
random_all_encodings random_codepoints_all_formats_to_files(std::filesystem::path fp, int nlns, int ncp_ln) {
	std::string file_name_no_ext = fp.stem().string();
	std::string file_name_ext = fp.extension().string();

	// Generate utf32 & write file
	std::vector<std::uint32_t> ru32 = random_utf32_to_file(fp, nlns, ncp_ln);

	// Encode the random utf32 -> utf8
	std::filesystem::path u8_fp = fp.replace_filename(file_name_no_ext+"_u8"+file_name_ext);
	std::vector<std::uint8_t> ru8;
	auto it_u8 = std::back_inserter(ru8);
	for (std::uint32_t cp : ru32) {
		it_u8 = to_utf8(cp, it_u8);
	}
	bool success_u8 = writefile(u8_fp,{reinterpret_cast<std::byte*>(ru8.data()), ru8.size()});
	expect(success_u8);
	if (!success_u8) {
		return {};
	}

	// Encode the random utf32 -> utf16
	std::filesystem::path u16_fp = fp.replace_filename(file_name_no_ext+"_u16"+file_name_ext);
	std::vector<std::uint16_t> ru16;
	auto it_u16 = std::back_inserter(ru16);
	for (std::uint32_t cp : ru32) {
		it_u16 = to_utf16(cp, it_u16);
	}
	std::size_t nbytes_16 = (sizeof(std::uint16_t)/sizeof(std::byte))*ru16.size();
	bool success_u16 = writefile(u16_fp,{reinterpret_cast<std::byte*>(ru16.data()), nbytes_16});
	expect(success_u16);
	if (!success_u16) {
		return {};
	}

	return random_all_encodings {std::move(ru32), std::move(ru16), std::move(ru8)};
}

std::string u32_to_string(std::span<const std::uint32_t> u32, int max_cpl) {
	std::string s;
	s.reserve(u32.size());
	int curr_cpl {0};
	for (const std::uint32_t dw : u32) {
		constexpr int cpe = 10;  // chars per entry
		s += std::format("{:#08X}u, ", dw);
		curr_cpl += cpe;
		if (curr_cpl > (max_cpl-cpe)) {
			curr_cpl = 0;
			s.back() = '\n';  // replaces the ' ' after the  ','
		}
	}
	std::replace(s.begin(),s.end(),'X','x');
	s += std::format("\n");
	return s;
}

std::string u16_to_string(std::span<const std::uint16_t> u16, int max_cpl) {
	std::string s;
	s.reserve(u16.size());
	int curr_cpl {0};
	for (const std::uint16_t w : u16) {
		constexpr int cpe = 8;  // chars per entry
		s += std::format("{:#06X}u, ", w);
		curr_cpl += cpe;
		if (curr_cpl > (max_cpl-cpe)) {
			curr_cpl = 0;
			s.back() = '\n';  // replaces the ' ' after the  ','
		}
	}
	std::replace(s.begin(),s.end(),'X','x');
	s += std::format("\n");
	return s;
}

std::string u8_to_string(std::span<const std::uint8_t> u8, int max_cpl) {
	std::string s;
	s.reserve(u8.size());
	int curr_cpl {0};
	for (const std::uint8_t b : u8) {
		constexpr int cpe = 6;  // chars per entry
		s += std::format("{:#04X}u, ", b);
		curr_cpl += cpe;
		if (curr_cpl > (max_cpl-cpe)) {
			curr_cpl = 0;
			s.back() = '\n';  // replaces the ' ' after the  ','
		}
	}
	std::replace(s.begin(),s.end(),'X','x');
	s += std::format("\n");
	return s;
}

std::string split_near_num_chars(const std::string& s, const int n, const char sep) {
	std::string r;
	r.reserve(s.size() + s.size()/n + 1);
	auto it = s.cbegin();
	while (it != s.cend()) {
		int i=0;
		while (it!=s.cend() && (i<n || *it!=sep)) {
			r.push_back(*it);
			++it;
			++i;
		}
		if (it==s.cend()) { break; }

		// i>=n && *it==sep
		r.push_back('\n');
		++it;  // skip the sep char
		i = 0;
	}
	return r;
}


void random_utf16_wch2multib();


int main(int argc, char* argv[]) {
	/*if (argc != 2) {
		std::cout << "Expected a file path" << std::endl;
		return 1;
	}*/

	{
		//std::cout << codepoints_bytes_reversed() << std::endl;
		//std::cout << fwd_vs_reverse() << std::endl;
	}

	/*{
		random_all_encodings rcps = random_codepoints_all_formats_to_files("D:\\dev\\utflib\\data\\rcps.txt", 500, 20);
		std::cout << u32_to_string(rcps.u32, 90) << "\n\n";
		std::cout << u16_to_string(rcps.u16, 90) << "\n\n";
		std::cout << u8_to_string(rcps.u8, 90) << "\n\n";
		std::cout << std::endl;
		std::cout << std::endl;
	}*/

	//random_utf16_wch2multib();
	//utf16_iterate_sample();
	//random_utf16_to_and_from_utf32();
	
	//std::string ru8 = get_random_utf8_sequences();
	//std::cout << split_near_num_chars(ru8,81,' ');
	
	//print_random_utf16_sequences();
	//random_utf8_to_and_from_utf32();
	//iterate_sample();
	//random_utf16_file("D:\\dev\\utflib\\ru16.txt", 1, 10);
	

	/*std::filesystem::path file{argv[1]};
	std::vector<std::byte> file_data = readfile(file);
	std::span<const char> s {reinterpret_cast<const char*>(file_data.data()), file_data.size()};
	bool b = utfchk(s);*/

	//f();


	return 0;
}
