#include <cimple_cmd.hpp>
#include <cimple_diagnostics.hpp>
#include <test_util.hpp>

#include <cstdlib>
#include <filesystem>
#include <optional>

#include <boost/asio.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace cimple::testing {
namespace bp = boost::process;

class CimpleTest : public ::testing::Test {
protected:
  void SetUp(std::string_view name) {
    test_name = name;

    std::filesystem::path resource_dir =
        std::filesystem::path(std::getenv("CIMPLE_TEST_DATA_DIR"));
    project_dir = resource_dir / test_name;
    build_dir = project_dir / "build";
  }

  void TearDown() override { std::filesystem::remove_all(build_dir); }

  void assert_no_issues() {
    for (const auto &message : Diagnostics::history) {
      ASSERT_EQ(message.type, Diagnostics::DiagnosticType::Info);
    }
  }

  void assert_program_output(
      const std::filesystem::path &program, int exit_code,
      const std::optional<std::string_view> &stdout_expected = std::nullopt,
      const std::optional<std::string_view> &stderr_expected = std::nullopt) {
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

  std::string test_name;
  std::filesystem::path project_dir;
  std::filesystem::path build_dir;
};

TEST_F(CimpleTest, basic) {
  SetUp("basic");

  // GIVEN: a simple hello world program
  // WHEN: build is called on it
  cmd::build(project_dir);

  // THEN: the build should finish successfully
  assert_no_issues();

  // THEN: the built binary should print "Hello World" and exit 142
  assert_program_output((project_dir / "basic"), 42);
}
} // namespace cimple::testing
