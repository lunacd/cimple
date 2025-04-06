#include <cimple_build.hpp>
#include <cimple_build_pkg.hpp>
#include <cimple_cmd.hpp>
#include <cimple_config.hpp>
#include <cimple_pkg_config.hpp>

namespace cimple::cmd {
void build(const std::filesystem::path &project_dir) {
  const Config config = load_config(project_dir / "cimple.toml");
  cimple::build(config);
}

void build_pkg(const std::filesystem::path &pkg_dir) {
  const auto config = load_pkg_config(pkg_dir / "pkg.toml");
  PkgBuildContext pkg_build_context{config};
  pkg_build_context.build_pkg();
}
} // namespace cimple::cmd
