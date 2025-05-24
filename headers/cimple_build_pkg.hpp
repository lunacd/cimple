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

  //! Run a package's build rules
  /*!
   * \param rules the rules to run
   * \param image_path path to an extracted chroot image
   * \param input_path path to extracted package source
   */
  void run_pkg_rules(const PkgRules &rules,
                     const std::filesystem::path &image_path,
                     const std::filesystem::path &input_path);
};
} // namespace cimple
