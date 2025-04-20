#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace cimple {
class Target {
public:
  std::string name;
  std::filesystem::path base_dir;
  std::filesystem::path header_dir;
  std::filesystem::path source_dir;
};

class Config {
public:
  std::string project_name;

  std::vector<Target> targets;

  std::filesystem::path project_dir;
  std::filesystem::path build_dir;

  std::filesystem::path c_compiler;
  std::filesystem::path cxx_compiler;

  std::vector<std::string> c_flags;
  std::vector<std::string> cxx_flags;

  std::vector<std::string> c_extensions;
  std::vector<std::string> cxx_extensions;
  std::vector<std::string> header_extensions;
};

Config load_config(const std::filesystem::path &config_path);
} // namespace cimple
