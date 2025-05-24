#include <cimple_build_pkg.hpp>
#include <cimple_constants.hpp>
#include <cimple_oci.hpp>
#include <cimple_pkg_config.hpp>
#include <cimple_requests.hpp>
#include <cimple_tar.hpp>
#include <cimple_util.hpp>

#include <iostream>
#include <string>

#include <cpp-subprocess/subprocess.hpp>

namespace cimple {
void PkgBuildContext::run_pkg_rules(const PkgRules &rules,
                                    const std::filesystem::path &image_path,
                                    const std::filesystem::path &input_path) {
  for (const auto &rule : rules) {
    std::vector<std::string> cmd;
    cmd.emplace_back(rule.program);
    cmd.insert(cmd.end(), rule.args.begin(), rule.args.end());

    auto p = subprocess::Popen(cmd, subprocess::environment(m_config.env),
                               subprocess::cwd(input_path));
    const auto res = p.communicate();
    std::cout << res.first.string() << "\n";
    std::cerr << res.second.string() << "\n";
  }
}

PkgBuildContext::PkgBuildContext(PkgConfig config)
    : m_config(std::move(config)),
      // TODO: get a real temp dir
      m_temp_dir("D:/temp") {}

PkgBuildContext::~PkgBuildContext() {}

void PkgBuildContext::build_pkg() {
  Curl curl;

  // Download image tarball
  const auto image_tarball_path =
      m_temp_dir / "windows-x86_64-bootstrap-msys.tar.gz";
  curl.download_file(
      std::format("{}/image/windows-x86_64-bootstrap-msys.tar.gz",
                  constants::cimple_pi_root),
      image_tarball_path);
  // Set up image directory
  extract_tar(image_tarball_path, m_config.image_path);

  // Download input tarball
  // TODO: actually take this from config
  const auto input_tarball_path = m_temp_dir / "bash-5.2.tar.gz";
  curl.download_file("https://ftp.gnu.org/gnu/bash/bash-5.2.tar.gz",
                     input_tarball_path);

  // Set up build dir
  // - data/ <=== sneak arbitrary data into the build dir
  // - input/ <=== patched source
  // - output/ <=== everything here is packaged
  // TODO: extract to different directory
  extract_tar(input_tarball_path, m_config.input_path);

  run_pkg_rules(m_config.default_rules, m_config.image_path,
                m_config.input_path);
}
} // namespace cimple
