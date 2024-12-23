#pragma once
#include <vector>
#include <filesystem>

std::vector<std::byte> readfile(const std::filesystem::path& fp);

