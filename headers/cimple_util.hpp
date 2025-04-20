#pragma once

#include <concepts>
#include <filesystem>
#include <vector>

namespace cimple {
template <typename T>
concept is_string_type = requires(T str) {
  typename T::value_type;
  { str.begin() } -> std::same_as<typename T::iterator>;
  { str.find(*str.begin()) } -> std::same_as<typename T::size_type>;
  { str.find(*str.begin(), 0) } -> std::same_as<typename T::size_type>;
};

template <typename T>
  requires is_string_type<T>
std::vector<T> split(T str, typename T::value_type delim) {
  std::vector<T> result;
  size_t current_delim = 0;
  size_t start = 0;
  size_t next = str.find(delim, start);
  while (current_delim != T::npos) {
    const auto segment = str.substr(start, next - start);
    if (segment.size() > 0) {
      result.emplace_back(segment);
    }
    current_delim = next;
    start = current_delim + 1;
    next = str.find(delim, start);
  }
  return result;
}

std::vector<std::filesystem::path>
files_with_extension(const std::filesystem::path &dir,
                     const std::vector<std::string> &extensions);

std::wstring to_wstring(const std::string &str);
}; // namespace cimple
