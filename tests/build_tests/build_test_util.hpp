#pragma once

#include <filesystem>

#include <gtest/gtest.h>

namespace cimple::testing {
class CimpleBuildTest : public ::testing::Test {
protected:
  void SetUp(std::string_view name);

  void TearDown() override;

  void assert_program_output(
      const std::filesystem::path &program, int exit_code,
      const std::optional<std::string_view> &stdout_expected = std::nullopt,
      const std::optional<std::string_view> &stderr_expected =
          std::nullopt) const;

  std::filesystem::path
  get_program_path(const std::filesystem::path &path) const;

  std::string test_name;
  std::filesystem::path project_dir;
  std::filesystem::path build_dir;
};
} // namespace cimple::testing
