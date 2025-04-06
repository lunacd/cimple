#include <cimple_diagnostics.hpp>
#include <test_util.hpp>

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

std::filesystem::path
get_test_data_path(const std::filesystem::path &data_path) {
  const auto resource_dir_raw = std::getenv("CIMPLE_TEST_DATA_DIR");
  assert(resource_dir_raw != nullptr);
  std::filesystem::path resource_dir = std::filesystem::path(resource_dir_raw);
  return resource_dir / data_path;
}

void cimple_assert_no_issues() {
  for (const auto &message : Diagnostics::history) {
    ASSERT_EQ(message.type, Diagnostics::DiagnosticType::Info);
  }
}
} // namespace cimple::testing
