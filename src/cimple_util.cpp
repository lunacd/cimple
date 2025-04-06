#include <cimple_util.hpp>

#include <algorithm>
#include <filesystem>
#include <string_view>
#include <vector>


#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace cimple {
std::vector<std::string_view> split(std::string_view str, char delim) {
  std::vector<std::string_view> result;
  size_t current_delim = 0;
  size_t start = 0;
  size_t next = str.find(delim, start);
  while (current_delim != std::string_view::npos) {
    const auto segment = str.substr(start, next - start);
    if (segment.size() > 0) {
      result.emplace_back(segment);
    }
    current_delim = next;
    start = current_delim + 1;
    next = str.find(delim, start);
  }
  return result;
};

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
