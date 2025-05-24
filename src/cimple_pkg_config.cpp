#include <cimple_pkg_config.hpp>
#include <cimple_util.hpp>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>

#include <cpp-subprocess/subprocess.hpp>
#include <toml++/toml.hpp>

namespace cimple {
namespace {
toml::table get_table(const toml::table &table, std::string_view key) {
  const auto result = table[key].as_table();
  if (result == nullptr) {
    throw std::runtime_error(
        std::format("{} key is required for package config", key));
  }
  return *result;
}

std::optional<PkgCommand> get_package_cmd(const std::string &cmd) {
  if (cmd.empty()) {
    return std::nullopt;
  }
  const auto split_cmd = split(cmd, std::string(" "));
  assert(!split_cmd.empty());
  const auto begin = split_cmd.begin();
  std::vector<std::string> args(std::next(begin), split_cmd.end());
  return PkgCommand{.program = std::string(*begin), .args = args};
}

PkgRules get_package_rules(const toml::array *arr) {
  if (arr == nullptr) {
    throw std::runtime_error("rules must be a list of commands");
  }
  PkgRules rules;
  for (const auto &item : *arr) {
    const auto item_str = item.value<std::string>();
    // Item is a string
    if (item_str) {
      const auto cmd = get_package_cmd(item_str.value());
      if (cmd) {
        rules.emplace_back(cmd.value());
      }
      continue;
    }
    // Then the item has to be an array
    const auto arr = item.as_array();
    if (arr == nullptr) {
      throw std::runtime_error(
          "a command has to either be a string or an array of strings");
    }
    const auto begin = arr->begin();
    if (begin == arr->end()) {
      continue;
    }
    const auto program = begin->value<std::string>();
    if (!program) {
      throw std::runtime_error(
          "a command has to either be a string or an array of strings");
    }
    std::vector<std::string> args;
    std::transform(
        std::next(begin), arr->end(), std::back_inserter(args),
        [](const toml::node &item) {
          const auto arg = item.value<std::string>();
          if (!arg) {
            throw std::runtime_error(
                "a command has to either be a string or an array of strings");
          }
          return arg.value();
        });
    rules.emplace_back(program.value(), args);
  }
  return rules;
}

//! Get MSVC environment variables
/*!
 * \return environment with MSVC variables populated
 */
subprocess::env_map_t get_msvc_env(const subprocess::env_map_t &original_env) {
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
       "ForEach-Object { \"$($_.Name)=$($_.Value)\" } }"},
      subprocess::output{subprocess::PIPE}, subprocess::environment{envvars});
  // TODO: Check result?
  // TODO: original env seems to be leaking into the result, either isolate it,
  // or filter this somehow
  const auto process_result = p.communicate();
  const auto out_str = to_wstring(process_result.first.string());

  // Parse envvars
  subprocess::env_str_t deliminator = L"\r\n";

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
    path_it->second = original_env.at(L"Path") + L";" + path_it->second;
  }
  for (auto &[env_key, env_value] : original_env) {
    if (env_key == L"Path") {
      continue;
    }
    envvars.emplace(env_key, env_value);
  }
  return envvars;
}
}; // namespace

PkgConfig load_pkg_config(const std::filesystem::path &config_path) {
  auto config = toml::parse_file(config_path.generic_string());

  // pkg section
  const auto pkg_sec = get_table(config, "pkg");
  const auto pkg_name = pkg_sec["name"].value<std::string>();
  if (!pkg_name) {
    throw std::runtime_error("pkg.name is required for package config");
  }
  const auto supported_platforms_arr =
      pkg_sec["supported_platforms"].as_array();
  if (supported_platforms_arr == nullptr) {
    throw std::runtime_error(
        "pkg.supported_platforms is required for package config");
  }
  std::vector<std::string> supported_platforms;
  for (const auto &supported_platform : *supported_platforms_arr) {
    const auto supported_platform_str = supported_platform.value<std::string>();
    if (!supported_platform_str) {
      throw std::runtime_error("string expected for supported_platforms");
    }
    supported_platforms.emplace_back(supported_platform_str.value());
  }

  // input section
  const auto input_sec = get_table(config, "input");
  const auto input_url = input_sec["url"].value<std::string>();
  if (!input_url) {
    throw std::runtime_error("input.url is required for package config");
  }

  // output section
  const auto output_sec = get_table(config, "output");
  const auto output_format = output_sec["format_version"].value<int64_t>();
  if (!output_format) {
    throw std::runtime_error("output.format is required for package config");
  }

  // rules section
  const auto rules_sec = get_table(config, "rules");
  const auto default_rules_arr = rules_sec["default"].as_array();
  PkgRules default_rules = get_package_rules(default_rules_arr);
  std::vector<PkgOverrideRules> override_rules_vec;
  for (const auto [k, v] : rules_sec) {
    if (k == "default") {
      continue;
    }
    PkgOverrideRules override_rules{.platform_matcher = std::regex(k.data()),
                                    .rules = get_package_rules(v.as_array())};
    override_rules_vec.emplace_back(override_rules);
  }

  // TODO: consider possibility for reuse?
  // TODO: extract to different directory
  // TODO: respect `image` setting in package manifest
  const std::filesystem::path image_path{"D:/temp/image/"};
  const std::filesystem::path input_path{"D:/temp/input/"};

  // Join paths by bp::environment::delimiter
  // TODO: remove hard-coded msys path after bootstrapping coreutils
  std::vector<std::filesystem::path> env_path{
      image_path / "bin",
      image_path / "usr" / "bin",
  };

  subprocess::env_str_t env_path_str;
  subprocess::env_char_t path_delimiter =
#ifdef _WIN32
      L';'
#else
      ':'
#endif
      ;
  std::ranges::copy(
      env_path | std::views::transform([](const std::filesystem::path &path) {
        return path.generic_string();
      }) | std::views::join_with(path_delimiter),
      std::back_inserter(env_path_str));
  // TODO: remove hard-coded CC and CXX after there's support for env in config
  subprocess::env_map_t env{
#ifdef _WIN32
      {L"Path", env_path_str.c_str()},
#else
      {"PATH", env_path_str.c_str()},
#endif
  };

#ifdef _WIN32
  // TODO: replace hard-coded D:/temp, this should probably go into
  // image-config.json
  env.emplace(L"TMP", L"D:/temp/tmp");
  env.emplace(L"TEMP", L"D:/temp/tmp");

  // Get MSVC environment
  // TODO: should this step be made configurable?
  env = get_msvc_env(env);
#endif

  return PkgConfig{.name = pkg_name.value(),
                   .supported_platforms = supported_platforms,
                   .input = input_url.value(),
                   .output_format = output_format.value(),
                   .default_rules = default_rules,
                   .override_rules = override_rules_vec,
                   .image_path = image_path,
                   .input_path = input_path,
                   .env = env};
}

} // namespace cimple
