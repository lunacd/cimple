#pragma once

#include <string>

#include <boost/asio/readable_pipe.hpp>

namespace cimple::testing {
std::string read_pipe(boost::asio::readable_pipe &rp);
}