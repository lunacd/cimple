#pragma once

#include <cimple_pkg_config.hpp>

#include <string_view>
#include <string>
#include <vector>

#include <boost/asio/io_context.hpp>
extern "C" {
#include <lua.h>
}

namespace cimple {
class PkgBuildContext {
public:
  PkgBuildContext(PkgConfig config);
  ~PkgBuildContext();

  void build_pkg();

private:
  boost::asio::io_context m_io_context;
  lua_State *m_lua_state;
  PkgConfig m_config;
  std::filesystem::path m_temp_dir;

  int run_process(const std::string_view &program,
                  const std::vector<std::string> &args);

  int l_run(lua_State *lua_state);
  int l_run_full(lua_State *lua_state);

  friend int open_cimple_lib(lua_State *L);
};
} // namespace cimple
