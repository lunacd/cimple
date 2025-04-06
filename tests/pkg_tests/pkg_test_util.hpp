#pragma once

#include <filesystem>

#include <gtest/gtest.h>

namespace cimple::testing {
class CimplePkgTest : public ::testing::Test {
protected:
  void SetUp(std::string_view name);

  void TearDown() override;

  std::filesystem::path pkg_dir;
};
} // namespace cimple::testing