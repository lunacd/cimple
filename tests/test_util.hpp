#pragma once

#include <filesystem>
#include <string>
#include <tuple>

#include <boost/asio/readable_pipe.hpp>
#include <gtest/gtest.h>

namespace cimple::testing {
std::string read_pipe(boost::asio::readable_pipe &rp);

std::filesystem::path
get_test_data_path(const std::filesystem::path &data_path);

void cimple_assert_no_issues();
} // namespace cimple::testing