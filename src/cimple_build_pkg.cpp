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
subprocess::env_map_t PkgBuildContext::get_msvc_env() {
  // Start with an empty environment
  subprocess::env_map_t envvars;
  // Run vcvarsall.bat
  auto p = subprocess::Popen(
      {"C:\\WINDOWS\\System32\\WindowsPowerShell\\v1.0\\powershell.exe",
       "-Command",
       "&{Import-Module 'C:\\Program Files\\Microsoft Visual "
       "Studio\\2022\\Community\\Common7\\Tools\\Microsoft.VisualStudio."
       "DevShell.dll'; Enter-VsDevShell 393607b9 -SkipAutomaticLocation "
       "-DevCmdArguments '-arch=x64 -host_arch=x64'; Get-ChildItem Env: | "
       "ForEach-Object { \\\"$($_.Name)=$($_.Value)\\\" } }"},
      subprocess::output{subprocess::PIPE}, subprocess::environment{envvars});
  const auto stdout_buf = p.communicate().first;
  const auto out_str =
#ifdef _WIN32
      to_wstring(stdout_buf.string());
#else
      stdout_buf.string();
#endif

  // Parse envvars
  subprocess::env_str_t deliminator =
#ifdef _WIN32
      L"\r\n"
#else
      "\n"
#endif
      ;
  const auto lines = split(out_str, deliminator);
  for (const auto &line : lines) {
    const auto equal_sign = line.find('=');
    if (equal_sign == std::string_view::npos) {
      continue;
    }
    // It's a envvar if there's a equal sign
    const auto key = line.substr(0, equal_sign);
    const auto value = line.substr(equal_sign + 1);
    envvars.emplace(key, value);
  }
  // Merge cimple-specific env into VS ones
  const auto path_it = envvars.find(L"Path");
  if (path_it != envvars.end()) {
    path_it->second = m_config.env.at(L"Path") + L";" + path_it->second;
  }
  for (auto &[env_key, env_value] : m_config.env) {
    if (env_key == L"Path") {
      continue;
    }
    envvars.emplace(env_key, env_value);
  }
  return envvars;
}

void PkgBuildContext::run_pkg_rules(const PkgRules &rules,
                                    const std::filesystem::path &image_path,
                                    const std::filesystem::path &input_path) {
  const auto env =
#ifdef _WIN32
      get_msvc_env()
#else
      m_config.env
#endif
      ;

  for (const auto &rule : rules) {
    std::vector<std::string> cmd;
    cmd.emplace_back(rule.program);
    cmd.insert(cmd.end(), rule.args.begin(), rule.args.end());

    auto p = subprocess::Popen(cmd, subprocess::environment(env),
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
