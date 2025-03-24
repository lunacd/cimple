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
  return Target{
      .name = std::string{name},
      .base_dir = project_dir,
      .header_dir = project_dir / "headers",
      .source_dir = project_dir / "src",
  };
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
  return targets;
};

Config load_config(const std::filesystem::path &config_path) {
  auto config = toml::parse_file(config_path.generic_string());
  const auto project_dir = config_path.parent_path();

  // Project name
  auto project_name = config["project"]["name"].value<std::string>();
  if (!project_name) {
    throw std::runtime_error("project.name must be specified in cimple.toml");
  }

  // Load targets
  auto targets_table = config["target"].as_table();
  std::vector<Target> targets = targets_table
                                    ? load_targets(*targets_table, project_dir)
                                    : std::vector<Target>{};

  // Default target of project name
  if (targets.size() == 0) {
    targets.emplace_back(Target{
        .name = project_name.value(),
        .base_dir = project_dir,
        .header_dir = project_dir / "headers",
        .source_dir = project_dir / "src",
    });
  }

  // TODO: provide a clang toolchain.
  // TODO: make this configurable.
  const auto c_compiler =
      std::filesystem::path("C:/Program Files/LLVM/bin/clang.exe");
  const auto cxx_compiler =
      std::filesystem::path("C:/Program Files/LLVM/bin/clang++.exe");

  return Config{.targets = targets,
                .project_dir = project_dir,
                // TODO: make build dir configurable
                .build_dir = project_dir / "build",
                .c_compiler = c_compiler,
                .cxx_compiler = cxx_compiler,
                .c_flags = {},
                .cxx_flags = {},
                .c_extensions = {".c"},
                .cxx_extensions = {".cpp"},
                .header_extensions = {".h", ".hpp"}};
}
} // namespace cimple
