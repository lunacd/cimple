#pragma once

#include <filesystem>
#include <vector>

namespace cimple {
class NinjaGraph {
public:
  NinjaGraph(std::filesystem::path c_compiler,
             std::filesystem::path cxx_compiler)
      : m_c_compiler(std::move(c_compiler)),
        m_cxx_compiler(std::move(cxx_compiler)) {}

  enum class Rule { Compile, Link };

  void add_build(const std::vector<std::filesystem::path> &input,
                 const std::filesystem::path &output, Rule rule);

  void export_ninja(const std::filesystem::path &path) const;

private:
  class Build {
  public:
    std::vector<std::filesystem::path> input;
    std::filesystem::path output;
    Rule rule;
  };

  static std::string escape_path(const std::filesystem::path &path);

  std::vector<Build> m_builds;
  std::filesystem::path m_cxx_compiler;
  std::filesystem::path m_c_compiler;
};
} // namespace cimple
