#include <cimple_build_pkg.hpp>
#include <cimple_pkg_config.hpp>
#include <cimple_requests.hpp>
#include <cimple_tar.hpp>
#include <cimple_util.hpp>


#include <stdexcept>
#include <string>
#include <string_view>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

#include <boost/process/v2/environment.hpp>
#include <boost/process/v2/process.hpp>

namespace cimple {
namespace bp = boost::process;

typedef int (PkgBuildContext::*MemberFunc)(lua_State *L);

template <MemberFunc func> int dispatch_member_func(lua_State *L) {
  PkgBuildContext *instance_ptr =
      *static_cast<PkgBuildContext **>(lua_getextraspace(L));
  return ((*instance_ptr).*func)(L);
}

int open_cimple_lib(lua_State *L) {
  const luaL_Reg lua_regs[] = {
      {"run", &dispatch_member_func<&PkgBuildContext::l_run>},
      {"run_full", &dispatch_member_func<&PkgBuildContext::l_run_full>},
      {NULL, NULL}};
  luaL_newlib(L, lua_regs);

  return 1;
}

PkgBuildContext::PkgBuildContext(PkgConfig config)
    : m_config(std::move(config)),
      // TODO: get a real temp dir
      m_temp_dir("D:/temp") {
  m_lua_state = luaL_newstate();
  luaL_openlibs(m_lua_state);

  // Store pointer to this instance
  *static_cast<PkgBuildContext **>(lua_getextraspace(m_lua_state)) = this;

  // Register lua functions
  luaL_requiref(m_lua_state, "cimple", open_cimple_lib, true);
}

PkgBuildContext::~PkgBuildContext() { lua_close(m_lua_state); }

int PkgBuildContext::run_process(const std::string_view &program,
                                 const std::vector<std::string> &args) {
  // Append .exe on Windows
  std::string program_name = std::string{program}
#ifdef _WIN32
                             + ".exe"
#endif
      ;

  const auto program_path =
      bp::environment::find_executable(program_name, m_config.env);
  if (program_path.empty()) {
    luaL_error(m_lua_state, "Cannot find executable %s",
               std::string(program).c_str());
  }
  bp::process child{m_io_context, program_path, args};

  child.wait();
  return child.exit_code();
}

/**
 * l_run_full
 *
 * lua name: cimple.run_full
 *
 * args:
 * - command: an array of strings
 *
 * returns:
 * - exit_code: number
 */
int PkgBuildContext::l_run_full(lua_State *L) {
  if (!lua_istable(L, 1)) {
    luaL_error(L, "Invalid argument type to run_full, expecting array");
    return 0;
  }

  lua_pushnil(L); // First key for iteration
  int has_value = lua_next(L, 1);
  if (!has_value) {
    luaL_error(L, "Argument to run_full has to have at least one element");
    return 0;
  }
  size_t program_size;
  const char *program_raw = luaL_checklstring(L, -1, &program_size);
  std::string program{program_raw, program_size};
  lua_pop(L, 1); // pop value

  std::vector<std::string> args;
  while (lua_next(L, 1)) {
    // next key at -2, value at -1
    size_t arg_size;
    const char *arg_raw = luaL_checklstring(L, -1, &arg_size);
    args.emplace_back(arg_raw, arg_size);
    lua_pop(L, 1); // pop value
  }

  lua_pop(L, 1); // pop key

  int exit_code = run_process(program, args);
  lua_pushnumber(L, exit_code);

  return 1;
}

/**
 * l_run
 *
 * lua name: cimple.run
 *
 * args:
 * - command(s): string
 *   The given string is first split by new lines and then by spaces and
 *   executed without further processing. Does NOT support any shell
 *   functionalities. Does NOT support quoting. If your command requires
 *   quoting, use run_full instead.
 *
 * returns:
 * - exit_code: number
 */
int PkgBuildContext::l_run(lua_State *L) {
  // Get argument
  size_t length;
  const char *commands_raw = luaL_checklstring(L, 1, &length);
  std::string commands_str = std::string(commands_raw, length);

  // Split program and args
  int exit_code = 0;
  std::vector<std::string_view> commands = split(commands_str, '\n');
  for (const auto command : commands) {
    std::vector<std::string_view> split_command = split(command, ' ');
    if (split_command.empty()) {
      continue;
    }
    std::vector<std::string> args{++split_command.begin(), split_command.end()};
    exit_code = run_process(split_command[0], args);
    if (exit_code != 0) {
      break;
    }
  }

  lua_pushnumber(L, exit_code);
  return 1;
}

void PkgBuildContext::build_pkg() {
  // Download input tarball
  Curl curl;
  // TODO: actually take this from config
  curl.download_file("https://ftp.gnu.org/gnu/m4/m4-1.4.19.tar.gz",
                     m_temp_dir / "m4-1.4.19.tar.gz");

  // Set up build dir
  // - data/ <=== sneak arbitrary data into the build dir
  // - input/ <=== patched source
  // - output/ <=== everything here is packaged
  extract_tar(m_temp_dir / "m4-1.4.19.tar.gz", m_temp_dir / "input");

  const auto handle_lua_error = [this](std::string_view comment) {
    size_t msg_size;
    const char *err_msg = luaL_checklstring(m_lua_state, -1, &msg_size);
    throw std::runtime_error(
        std::format("{}: {}", comment, std::string(err_msg, msg_size)));
  };

  if (luaL_loadfile(m_lua_state,
                    m_config.build_script.generic_string().c_str()) != LUA_OK) {
    handle_lua_error("Error loading pkg.lua");
  }
  if (lua_pcall(m_lua_state, 0, 1, 0) != LUA_OK) {
    handle_lua_error("Error running pkg.lua");
  }
  const auto result = lua_tonumber(m_lua_state, -1);
  // Finished successfully, pop all from stack
  lua_pop(m_lua_state, lua_gettop(m_lua_state));
}
} // namespace cimple
