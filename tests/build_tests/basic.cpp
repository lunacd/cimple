#include <build_tests/build_test_util.hpp>
#include <cimple_cmd.hpp>
#include <test_util.hpp>

#include <filesystem>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace cimple::testing {
TEST_F(CimpleBuildTest, basic) {
  SetUp("basic");

  // GIVEN: a simple hello world program
  // WHEN: build is called on it
  cmd::build(project_dir);

  // THEN: the build should finish successfully
  cimple_assert_no_issues();

  // THEN: the build dir should be created
  ASSERT_TRUE(std::filesystem::exists(build_dir));

  // THEN: the built binary should print "Hello World" and exit 142
  std::filesystem::path program_path = get_program_path(build_dir / "basic");
  ASSERT_TRUE(std::filesystem::exists(program_path));
  assert_program_output(program_path, 148);
}
} // namespace cimple::testing
