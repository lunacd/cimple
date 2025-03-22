#include <test_util.hpp>
#include <cimple_diagnostics.hpp>

#include <filesystem>
#include <stdexcept>

#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>
#include <gtest/gtest.h>

namespace cimple::testing {
namespace bp = boost::process;

std::string read_pipe(boost::asio::readable_pipe &rp) {
  boost::asio::streambuf buffer;
  boost::system::error_code error;
  boost::asio::read(rp, buffer, boost::asio::transfer_all(), error);

  if (error && error != boost::asio::error::eof) {
    throw std::runtime_error("Failed to read from program output pipe.");
  }

  return std::string(boost::asio::buffers_begin(buffer.data()),
                     boost::asio::buffers_end(buffer.data()));
}

void CimpleTest::SetUp(std::string_view name) {
  test_name = name;

  const auto resource_dir_raw = std::getenv("CIMPLE_TEST_DATA_DIR");
  ASSERT_TRUE(resource_dir_raw)
      << "Expecting CIMPLE_TEST_DATA_DIR environment to be set during test.";
  std::filesystem::path resource_dir = std::filesystem::path(resource_dir_raw);
  project_dir = resource_dir / test_name;
  build_dir = project_dir / "build";
}

void CimpleTest::TearDown() { std::filesystem::remove_all(build_dir); }

void CimpleTest::assert_no_issues() {
  for (const auto &message : Diagnostics::history) {
    ASSERT_EQ(message.type, Diagnostics::DiagnosticType::Info);
  }
}

void CimpleTest::assert_program_output(
    const std::filesystem::path &program, int exit_code,
    const std::optional<std::string_view> &stdout_expected,
    const std::optional<std::string_view> &stderr_expected) {
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

} // namespace cimple::testing