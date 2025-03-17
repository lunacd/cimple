#include <cimple_util.hpp>

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <optional>
#include <string_view>
#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace cimple {
std::vector<std::string_view> split(std::string_view str, char delim) {
  size_t last = 0, next = 0;
  std::vector<std::string_view> out;

  while ((next = str.find(delim, last)) != std::string_view::npos) {
    out.emplace_back(str.substr(last, next - last));
    last = next + 1;
  }
  out.emplace_back(str.substr(last));

  return out;
};

bool can_execute(const std::filesystem::path &path) {
#ifdef _WIN32
  if (_waccess(path.c_str(), 4) == 0) {
    return true;
  }
  return false;
#else
  return access(path.c_str(), X_OK) == 0;
#endif
}

std::optional<std::filesystem::path> which(std::string_view command) {
  const char path_sep =
#ifdef _WIN32
      ';'
#else
      ':'
#endif
      ;
  std::string path_env = std::getenv("PATH");
  std::vector<std::string_view> paths_str = split(path_env, path_sep);
  std::vector<std::filesystem::path> paths;
  std::transform(paths_str.begin(), paths_str.end(), std::back_inserter(paths),
                 [](const auto &str) { return std::filesystem::path{str}; });
  for (const auto &path : paths) {
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
      if (entry.path().filename() != command) {
        continue;
      }
      if (!can_execute(entry)) {
        continue;
      }
      return entry.path();
    }
  }
  return std::nullopt;
}

std::vector<std::filesystem::path>
files_with_extension(const std::filesystem::path &dir,
                     const std::vector<std::string> &extensions) {
  std::vector<std::filesystem::path> results;
  for (const auto &entry : std::filesystem::directory_iterator(dir)) {
    const auto ext = entry.path().extension();
    if (std::any_of(extensions.begin(), extensions.end(),
                    [&ext](const std::string &target_ext) {
                      return ext == target_ext;
                    })) {
      results.emplace_back(entry.path());
    }
  }
  return results;
}
}; // namespace cimple
