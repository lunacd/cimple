#include <algorithm>
#include <boost/process/v2/detail/environment_win.hpp>
#include <cimple_pkg_config.hpp>

#include <iterator>
#include <ranges>
#include <stdexcept>

#include <toml++/toml.hpp>
#include <unordered_map>

namespace cimple {
namespace bp = boost::process;

PkgConfig load_pkg_config(const std::filesystem::path &config_path) {
  auto config = toml::parse_file(config_path.generic_string());

  const auto input_sec = config["input"].as_table();
  if (input_sec == nullptr) {
    throw std::runtime_error("input key is required for package config");
  }
  const auto input_url = (*input_sec)["url"].value<std::string>();
  if (!input_url) {
    throw std::runtime_error("input.url is required for package config");
  }

  const std::filesystem::path toolchain_path{"D:/toolchain/"};
  const std::filesystem::path workdir{"D:/workdir/"};

  // Join paths by bp::environment::delimiter
  std::vector<std::filesystem::path> env_path{workdir / "bin",
                                              toolchain_path / "bin"};
  std::string env_path_str;
  std::ranges::copy(
      env_path | std::views::transform([](const std::filesystem::path &path) {
        return path.generic_string();
      }) | std::views::join_with(bp::environment::delimiter),
      std::back_inserter(env_path_str));
  std::unordered_map<bp::environment::key, bp::environment::value> env{
      {"PATH", env_path_str.c_str()}};

  return PkgConfig{.input = input_url.value(),
                   .build_script = config_path.parent_path() / "pkg.lua",
                   .toolchain_dir = toolchain_path,
                   .work_dir = workdir,
                   .env = env};
}
} // namespace cimple
