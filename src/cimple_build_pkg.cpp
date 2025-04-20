#include <cimple_build_pkg.hpp>
#include <cimple_pkg_config.hpp>
#include <cimple_requests.hpp>
#include <cimple_tar.hpp>
#include <cimple_util.hpp>

#include <iostream>
#include <string>
#include <string_view>

#include <cpp-subprocess/subprocess.hpp>

namespace cimple {
subprocess::env_map_t PkgBuildContext::get_msvc_env() {
  // Run vcvarsall.bat
  auto p = subprocess::Popen(
      {"C:\\WINDOWS\\System32\\WindowsPowerShell\\v1.0\\powershell.exe",
       "-Command",
       "&{Import-Module 'C:\\Program Files\\Microsoft Visual "
       "Studio\\2022\\Community\\Common7\\Tools\\Microsoft.VisualStudio."
       "DevShell.dll'; Enter-VsDevShell 393607b9 -SkipAutomaticLocation "
       "-DevCmdArguments '-arch=x64 -host_arch=x64'; Get-ChildItem Env: | "
       "ForEach-Object { \\\"$($_.Name)=$($_.Value)\\\" } }"},
      subprocess::output{subprocess::PIPE},
      subprocess::environment{m_config.env});
  const auto stdout_buf = p.communicate().first;
  const auto out_str =
#ifdef _WIN32
      to_wstring(stdout_buf.string());
#else
      stdout_buf.string();
#endif

  // Parse envvars
  subprocess::env_map_t envvars;
  subprocess::env_char_t deliminator =
#ifdef _WIN32
      L'\n'
#else
      '\n'
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
  return envvars;
}

void PkgBuildContext::run_pkg_rules(const PkgRules &rules,
                                    const std::filesystem::path &cwd) {
  const auto env =
#ifdef _WIN32
      get_msvc_env()
#else
      m_config.env
#endif
      ;
  for (const auto &rule : rules) {
    std::filesystem::path program;
    // TODO: remove hard-coded bash after bootstrapping bash
    if (rule.program == "bash") {
      program = "D:/msys64/usr/bin/bash.exe";
    } else {
      program = rule.program;
    }
    std::vector<std::string> cmd;
    cmd.emplace_back(program.generic_string());
    cmd.insert(cmd.end(), rule.args.begin(), rule.args.end());
    auto p = subprocess::Popen(cmd, subprocess::cwd{cwd},
                               subprocess::environment{env});
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
  // Download input tarball
  Curl curl;
  // TODO: actually take this from config
  curl.download_file("https://ftp.gnu.org/gnu/bash/bash-5.2.tar.gz",
                     m_temp_dir / "bash-5.2.tar.gz");

  // Set up build dir
  // - data/ <=== sneak arbitrary data into the build dir
  // - input/ <=== patched source
  // - output/ <=== everything here is packaged
  extract_tar(m_temp_dir / "bash-5.2.tar.gz", m_temp_dir / "input");

  run_pkg_rules(m_config.default_rules, m_temp_dir / "input");
}
} // namespace cimple
