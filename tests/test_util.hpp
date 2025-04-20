#pragma once

#include <filesystem>

#include <gtest/gtest.h>

namespace cimple::testing {
std::filesystem::path
get_test_data_path(const std::filesystem::path &data_path);

void cimple_assert_no_issues();
} // namespace cimple::testing