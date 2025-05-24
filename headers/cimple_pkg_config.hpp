#pragma once

#include <filesystem>
#include <regex>
#include <string>

#include <cpp-subprocess/subprocess.hpp>

namespace cimple {
class PkgCommand {
public:
  std::string program;
  std::vector<std::string> args;
};

using PkgRules = std::vector<PkgCommand>;

class PkgOverrideRules {
public:
  std::regex platform_matcher;
  PkgRules rules;
};

class PkgConfig {
public:
  std::string name;
  std::vector<std::string> supported_platforms;

  std::string input;
  std::string input_sha256;
  int64_t output_format;
  std::string output_sha256;

  PkgRules default_rules;
  std::vector<PkgOverrideRules> override_rules;

  std::filesystem::path image_path;
  std::filesystem::path input_path;

  subprocess::env_map_t env;
};

PkgConfig load_pkg_config(const std::filesystem::path &config_path);
} // namespace cimple
