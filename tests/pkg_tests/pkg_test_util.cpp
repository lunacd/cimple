#include <pkg_tests/pkg_test_util.hpp>
#include <test_util.hpp>

#include <string_view>

namespace cimple::testing {
void CimplePkgTest::SetUp(std::string_view name) {
  pkg_dir = get_test_data_path(std::filesystem::path("pkg_tests") / name);
}

void CimplePkgTest::TearDown() {}
} // namespace cimple::testing
