#include <cimple_util.hpp>

#include <algorithm>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace cimple {
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

std::wstring to_wstring(const std::string &str) {
#ifdef _WIN32
  if (str.empty())
    return std::wstring();

  size_t charsNeeded =
      ::MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);
  if (charsNeeded == 0)
    throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

  std::vector<wchar_t> buffer(charsNeeded);
  int charsConverted = ::MultiByteToWideChar(
      CP_UTF8, 0, str.data(), (int)str.size(), &buffer[0], buffer.size());
  if (charsConverted == 0)
    throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

  return std::wstring(buffer.begin(), buffer.end());
#else
  throw std::logic_error(
      "to_stringw is not implemented for POSIX platforms yet, plus you "
      "shouldn't need this in the first place.");
#endif
}
}; // namespace cimple
