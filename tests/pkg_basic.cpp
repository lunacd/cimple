#include "test_util.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cimple_cmd.hpp>

namespace cimple::testing {
TEST(pkg_basic, pkg_basic) {
  cimple::cmd::build_pkg(get_test_data_path("pkg_basic"));
}
} // namespace cimple::testing
