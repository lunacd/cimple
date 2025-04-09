#pragma once

#include <filesystem>

#include <curl/curl.h>

namespace cimple {
class Curl {
public:
  Curl();
  ~Curl();

  void download_file(const std::string &url, const std::filesystem::path &path);

private:
  class CurlHandle {
  public:
    CurlHandle();
    ~CurlHandle();

    CURL *curl_handle;
  };
};
} // namespace cimple
