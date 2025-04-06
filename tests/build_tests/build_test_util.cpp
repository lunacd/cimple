#include <build_tests/build_test_util.hpp>
#include <cimple_diagnostics.hpp>
#include <test_util.hpp>

#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>

namespace cimple::testing {
namespace bp = boost::process;

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
  boost::asio::io_context context;

  boost::asio::readable_pipe rp_out{context};
  boost::asio::readable_pipe rp_err{context};
  bp::process child(
      context, program, {},
      boost::process::process_stdio({.out = rp_out, .err = rp_err}));
  child.wait();

  ASSERT_EQ(child.exit_code(), exit_code);
  if (stdout_expected) {
    const auto stdout_content = read_pipe(rp_out);
    ASSERT_EQ(stdout_content, stdout_expected.value());
  }
  if (stderr_expected) {
    const auto stderr_content = read_pipe(rp_err);
    ASSERT_EQ(stderr_content, stderr_expected.value());
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
