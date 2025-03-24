#include "cimple_config.hpp"
#include <cimple_build.hpp>
#include <cimple_cmd.hpp>

namespace cimple::cmd {
void build(const std::filesystem::path &project_dir) {
  const Config config = load_config(project_dir / "cimple.toml");
  build(config);
}
} // namespace cimple::cmd
