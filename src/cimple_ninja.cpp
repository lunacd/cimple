#include <cimple_ninja.hpp>

#include <fstream>
#include <stdexcept>

namespace cimple {
void NinjaGraph::add_build(const std::vector<std::filesystem::path> &input,

                           const std::filesystem::path &output,
                           const NinjaGraph::Rule rule) {
  m_builds.emplace_back(input, output, rule);
}

void NinjaGraph::export_ninja(const std::filesystem::path &path) const {
  std::ofstream ninja_file{path};
  if (!ninja_file.is_open()) {
    throw std::runtime_error("Failed to open build.ninja file for writing");
  }

  // Rules
  // TODO: consider header dependencies
  ninja_file << "rule compile\n  command = \"" << m_cxx_compiler.generic_string()
             << "\" -c $in -o $out\n\n";
  ninja_file << "rule link\n  command = \"" << m_cxx_compiler.generic_string()
             << "\" $in -o $out\n\n";

  // Builds
  for (const auto &build : m_builds) {
    ninja_file << "build " << escape_path(build.output.generic_string()) << ": ";
    switch (build.rule) {
    case Rule::Compile:
      ninja_file << "compile";
      break;
    case Rule::Link:
      ninja_file << "link";
      break;
    }
    for (const auto &input : build.input) {
      ninja_file << " " << escape_path(input.generic_string());
    }
    ninja_file << "\n\n";
  }
}

std::string NinjaGraph::escape_path(const std::filesystem::path &path) {
#ifdef _WIN32
  std::string path_str = path.generic_string();
  // Windows absolute paths should have at most one colon
  const auto colon_pos = path_str.find(":");
  if (colon_pos != std::string::npos) {
    path_str.replace(colon_pos, 1, "$:");
  }
  return path_str;

#else
  return path.generic_string();
#endif
}
} // namespace cimple
