#include "utils.h"

#include <cstdint>
#include <vector>
#include <filesystem>
#include <cstdio>

std::vector<std::byte> readfile(const std::filesystem::path& fp) {
	std::vector<std::byte> file_data;

	if (!std::filesystem::exists(fp) || !std::filesystem::is_regular_file(fp)) {
		return file_data;
	}

	std::FILE* f = std::fopen(fp.string().c_str(), "rb");
	if (!f) {
		return file_data;
	}
	
	std::uintmax_t sz = std::filesystem::file_size(fp);
	file_data.resize(sz);
	std::size_t count_read = std::fread(file_data.data(), 1, sz, f);
	std::fclose(f);
	if (count_read != sz) {
		file_data.resize(0);
	}

	return file_data;
}
