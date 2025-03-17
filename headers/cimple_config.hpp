#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace cimple {
class Target {
public:
  std::string name;
  std::filesystem::path header_dir;
  std::filesystem::path source_dir;
};

class Config {
public:
  std::vector<Target> targets;

  std::filesystem::path project_dir;

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
