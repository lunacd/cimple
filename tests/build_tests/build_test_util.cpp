#include <build_tests/build_test_util.hpp>
#include <cimple_diagnostics.hpp>
#include <cimple_util.hpp>
#include <test_util.hpp>

#include <cpp-subprocess/subprocess.hpp>

namespace cimple::testing {
void CimpleBuildTest::SetUp(std::string_view name) {
  test_name = name;

  project_dir =
      get_test_data_path(std::filesystem::path("build_tests") / test_name);
  build_dir = project_dir / "build";
}

void CimpleBuildTest::TearDown() { std::filesystem::remove_all(build_dir); }

void CimpleBuildTest::assert_program_output(
    const std::filesystem::path &program, int exit_code,
    const std::optional<std::string_view> &stdout_expected,
    const std::optional<std::string_view> &stderr_expected) const {

  auto p = subprocess::Popen({program.generic_string()},
                             subprocess::output{subprocess::PIPE});
  const auto res = p.communicate();

  ASSERT_EQ(p.retcode(), exit_code);
  if (stdout_expected) {
    ASSERT_EQ(res.first.string(), stdout_expected.value());
  }
  if (stderr_expected) {
    ASSERT_EQ(res.second.string(), stderr_expected.value());
  }
}

std::filesystem::path
CimpleBuildTest::get_program_path(const std::filesystem::path &path) const {
#ifdef _WIN32
  auto new_path = path;
  new_path.replace_extension("exe");
  return new_path;
#else
  return path;
#endif
}
} // namespace cimple::testing
