#include "utils.h"

#include <cstdint>
#include <vector>
#include <filesystem>
#include <cstdio>
#include <span>

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


// Overwrites the file if it already exists
bool writefile(const std::filesystem::path& fp, std::span<const std::byte> data) {
	class file_raii {
	public:
		file_raii(std::FILE* fp) : m_fp(fp) {}
		~file_raii() {
			if (m_fp) {
				std::fclose(m_fp);
				m_fp = nullptr;
			}
		}
		operator bool() const {
			return m_fp;
		}
		operator std::FILE*() {
			return m_fp;
		}
	private:
		std::FILE* m_fp {};
	};

	file_raii f = std::fopen(fp.string().c_str(), "wb");
	if (!f) {
		return false;
	}

	std::size_t sz = std::fwrite(data.data(), sizeof(std::byte), data.size(), f);
	if (sz != data.size()) {
		return false;
	}

	return true;
}
