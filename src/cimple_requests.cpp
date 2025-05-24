#include <cimple_requests.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <curl/curl.h>
#include <curl/easy.h>

namespace cimple {
namespace {
size_t write_data_to_file(void *ptr, size_t size, size_t nmemb, void *stream) {
  std::ofstream *fstream = static_cast<std::ofstream *>(stream);
  fstream->write(static_cast<char *>(ptr), size * nmemb);
  return size * nmemb;
}
} // namespace

Curl::Curl() { curl_global_init(CURL_GLOBAL_ALL); }

Curl::~Curl() { curl_global_cleanup(); }

Curl::CurlHandle::CurlHandle() { curl_handle = curl_easy_init(); }

Curl::CurlHandle::~CurlHandle() {
  curl_easy_cleanup(curl_handle);
  curl_handle = nullptr;
}

void Curl::download_file(const std::string &url,
                         const std::filesystem::path &path) {
  CurlHandle handle;

  curl_easy_setopt(handle.curl_handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(handle.curl_handle, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(handle.curl_handle, CURLOPT_WRITEFUNCTION,
                   write_data_to_file);
  curl_easy_setopt(handle.curl_handle, CURLOPT_USERAGENT, "curl/8.12");

  // Follow redirects: CURLFOLLOW_OBEYCODE. Needed for downloads from GitHub release
  curl_easy_setopt(handle.curl_handle, CURLOPT_FOLLOWLOCATION,
                   2L);

  std::ofstream out_file;
  out_file.open(path.generic_string(), std::ios::out | std::ios::binary);
  if (!out_file) {
    throw std::runtime_error("Failed to open output file for writing");
  }
  curl_easy_setopt(handle.curl_handle, CURLOPT_WRITEDATA, &out_file);
  const auto error_code = curl_easy_perform(handle.curl_handle);
}
} // namespace cimple
