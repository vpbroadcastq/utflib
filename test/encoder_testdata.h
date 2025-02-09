#include <cstdint>
#include <span>
#include <vector>


struct encoder_testdata {
	std::vector<std::uint32_t> u32;
	std::vector<std::uint16_t> u16;
	std::vector<std::uint8_t> u8;
};

std::span<encoder_testdata> get_encoder_testdata_valid();
