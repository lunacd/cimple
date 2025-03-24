#include <cimple_build.hpp>
#include <cimple_config.hpp>
#include <cimple_ninja.hpp>
#include <cimple_util.hpp>

#include <filesystem>

#ifdef _WIN32
#include <winsock2.h>
#endif
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/start_dir.hpp>

namespace cimple {
namespace bp = boost::process;

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
  boost::asio::io_context context;
  std::filesystem::path ninja_path = "D:/msys64/usr/bin/ninja.exe";
  bp::process child(context, ninja_path, {},
                    bp::process_start_dir(config.build_dir));
  child.wait();
}
} // namespace cimple
