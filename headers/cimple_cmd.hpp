#pragma once

#include <filesystem>

namespace cimple::cmd {
void build(const std::filesystem::path &project_dir);

void build_pkg(const std::filesystem::path &pkg_dir);
} // namespace cimple::cmd
