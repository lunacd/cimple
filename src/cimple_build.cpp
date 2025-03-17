#include <cimple_build.hpp>
#include <cimple_config.hpp>
#include <cimple_util.hpp>


#include <filesystem>

#include <graaflib/graph.h>

namespace cimple {
using BuildGraph = graaf::directed_graph<std::filesystem::path, bool>;

void build_target_graph(const Target &target, const Config &config,
                        BuildGraph &build_grah) {
  const auto source_files =
      files_with_extension(target.source_dir, config.cxx_extensions);
}

void build(const Config &config) {
  graaf::directed_graph<std::filesystem::path, bool> build_graph;
  for (const auto &target : config.targets) {
    build_target_graph(target, config, build_graph);
  }
}
} // namespace cimple
