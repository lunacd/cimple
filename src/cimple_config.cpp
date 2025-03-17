#include <cimple_config.hpp>
#include <cimple_util.hpp>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <toml++/toml.hpp>

namespace cimple {
Target load_target(const std::string_view &name,
                   const toml::table &target_table,
                   const std::filesystem::path &project_dir) {
  return Target{.name = std::string{name},
                .header_dir = project_dir / "headers",
                .source_dir = project_dir / "src"};
}

std::vector<Target> load_targets(const toml::table &targets_table,
                                 const std::filesystem::path &project_dir) {
  std::vector<Target> targets;
  for (auto &&[target_name, target_config] : targets_table) {
    const auto target_table = target_config.as_table();
    if (!target_table) {
      continue;
    }
    targets.emplace_back(
        load_target(target_name.str(), *target_table, project_dir));
  }
  return std::vector<Target>{};
};

Config load_config(const std::filesystem::path &config_path) {
  auto config = toml::parse_file(config_path.generic_string());
  const auto project_dir = config_path.parent_path();

  // Load targets
  auto targets_table = config["target"].as_table();
  std::vector<Target> targets = targets_table
                                    ? load_targets(*targets_table, project_dir)
                                    : std::vector<Target>{};

  const auto c_compiler = which("clang");
  const auto cxx_compiler = which("clang++");

  if (!c_compiler || !cxx_compiler) {
    throw std::runtime_error("Failed to find C or C++ compiler");
  }

  return Config{.targets = targets,
                .project_dir = project_dir,
                .c_compiler = c_compiler.value(),
                .cxx_compiler = cxx_compiler.value(),
                .c_flags = {},
                .cxx_flags = {},
                .c_extensions = {".c"},
                .cxx_extensions = {".cpp"},
                .header_extensions = {".h", ".hpp"}};
}
} // namespace cimple
