#pragma once

#include <cimple_pkg_config.hpp>

#include <cpp-subprocess/subprocess.hpp>

namespace cimple {
class PkgBuildContext {
public:
  PkgBuildContext(PkgConfig config);
  ~PkgBuildContext();

  void build_pkg();

private:
  PkgConfig m_config;
  std::filesystem::path m_temp_dir;

  void run_pkg_rules(const PkgRules &rules, const std::filesystem::path &cwd);

  subprocess::env_map_t get_msvc_env();
};
} // namespace cimple
