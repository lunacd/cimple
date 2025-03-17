#pragma once

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

namespace cimple {
std::vector<std::string_view> split(std::string_view str, char delim);

bool can_execute(const std::filesystem::path &path);

std::optional<std::filesystem::path> which(std::string_view command);

std::vector<std::filesystem::path>
files_with_extension(const std::filesystem::path &dir,
                     const std::vector<std::string> &extensions);
}; // namespace cimple
