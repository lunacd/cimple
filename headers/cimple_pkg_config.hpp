#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include <boost/process/v2/environment.hpp>

namespace cimple {
class PkgConfig {
public:
  std::string input;
  std::string input_sha256;
  std::string output;
  std::string output_sha256;
  std::filesystem::path build_script;
  std::filesystem::path toolchain_dir;
  std::filesystem::path work_dir;

  std::unordered_map<boost::process::environment::key,
                     boost::process::environment::value>
      env;
};

PkgConfig load_pkg_config(const std::filesystem::path &config_path);
} // namespace cimple
