#include <cimple_diagnostics.hpp>
#include <test_util.hpp>

#include <gtest/gtest.h>

namespace cimple::testing {
std::filesystem::path
get_test_data_path(const std::filesystem::path &data_path) {
  const auto resource_dir_raw = std::getenv("CIMPLE_TEST_DATA_DIR");
  assert(resource_dir_raw != nullptr);
  std::filesystem::path resource_dir =
      std::filesystem::path(resource_dir_raw);
  return resource_dir / data_path;
}

void cimple_assert_no_issues() {
  for (const auto &message : Diagnostics::history) {
    ASSERT_EQ(message.type, Diagnostics::DiagnosticType::Info);
  }
}
} // namespace cimple::testing
