#include "utflib/utflib.h"
#include "utils.h"
#include "test.h"

#include <filesystem>
#include <iostream>
#include <format>

int main(int argc, char* argv[]) {
	/*if (argc != 2) {
		std::cout << "Expected a file path" << std::endl;
		return 1;
	}*/

	std::uint8_t val {0};
	std::string s;
	int nentries {0};
	while (true) {
		//std::format_to(std::ostream_iterator<char>(std::cout), std::format_string("{:#08b}, "), val);
		if (!((val>>7)==0b0 || (val>>5)==0b110 || (val>>4)==0b1110 || (val>>3)==0b11110)) {
			s += std::format("{:#010b}, ", val);
			++nentries;
			if (nentries%5 == 0) {
				s += std::format("\n");
			}
		}

		if (val == std::numeric_limits<std::uint8_t>::max()) {
			break;
		}
		++val;
	}
	
	std::cout << s << std::endl;

	test_utfchk();

	/*std::filesystem::path file{argv[1]};
	std::vector<std::byte> file_data = readfile(file);
	std::span<const char> s {reinterpret_cast<const char*>(file_data.data()), file_data.size()};
	bool b = utfchk(s);*/

	//f();

	return 0;
}
