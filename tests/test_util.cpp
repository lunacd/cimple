#include <boost/asio/buffers_iterator.hpp>
#include <test_util.hpp>

#include <stdexcept>

#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>

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
} // namespace cimple::testing