#pragma once

#include <filesystem>

namespace cimple {
void extract_tar(const std::filesystem::path &tar, const std::filesystem::path &target_path);
}
