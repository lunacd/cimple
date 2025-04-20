#include <cimple_build.hpp>
#include <cimple_config.hpp>
#include <cimple_ninja.hpp>
#include <cimple_util.hpp>

#include <cpp-subprocess/subprocess.hpp>

namespace cimple {
void build_target_graph(const Target &target, const Config &config,
                        NinjaGraph &build_grah) {
  const auto source_files =
      files_with_extension(target.source_dir, config.cxx_extensions);
  std::vector<std::filesystem::path> object_files;
  // Compile
  for (const auto &source_file : source_files) {
    auto build_target_rel =
        std::filesystem::relative(source_file, target.base_dir);
    auto build_target = config.build_dir / build_target_rel;

    build_target.replace_extension("o");
    object_files.emplace_back(build_target);
    build_grah.add_build({source_file}, build_target,
                         NinjaGraph::Rule::Compile);
  }
  // Link
  auto target_path = config.build_dir / target.name;
#ifdef _WIN32
  target_path.replace_extension("exe");
#endif
  build_grah.add_build(object_files, target_path, NinjaGraph::Rule::Link);
}

void build(const Config &config) {
  // Create build dir
  std::filesystem::create_directory(config.build_dir);

  // Create build graph
  NinjaGraph build_graph{config.c_compiler, config.cxx_compiler};
  for (const auto &target : config.targets) {
    build_target_graph(target, config, build_graph);
  }

  // TODO: export compilation database

  // Toolchain
  // TODO: provide a toolchain
  // TODO: make this configurable
  const std::filesystem::path cxx_compiler{
      "C:/Program Files/LLVM/bin/clang++.exe"};

  // Build
  build_graph.export_ninja(config.build_dir / "build.ninja");
  // TODO: provide ninja, and make this configurable
  auto p = subprocess::Popen({"D:/msys64/usr/bin/ninja.exe"},
                             subprocess::cwd{config.build_dir});
  p.wait();
}
} // namespace cimple
