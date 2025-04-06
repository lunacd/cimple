#include <cimple_cmd.hpp>
#include <pkg_tests/pkg_test_util.hpp>
#include <test_util.hpp>

namespace cimple::testing {
TEST_F(CimplePkgTest, basic) {
  SetUp("basic");

  // GIVEN: a simple package
  // WHEN: build_pkg is called on it
  cmd::build_pkg(pkg_dir);

  // THEN: there should be no issues
  cimple_assert_no_issues();
}
} // namespace cimple::testing
