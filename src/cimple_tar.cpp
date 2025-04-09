#include <cimple_diagnostics.hpp>
#include <cimple_tar.hpp>

#include <filesystem>
#include <stdexcept>

#include <archive.h>
#include <archive_entry.h>
#include <string>

namespace cimple {
namespace {
void handle_archive_error(struct archive *ar, int r) {
  if (r < ARCHIVE_OK && r >= ARCHIVE_WARN) {
    Diagnostics::warn(archive_error_string(ar));
  }
  if (r < ARCHIVE_WARN) {
    const char *msg = archive_error_string(ar);
    Diagnostics::error(msg);
    throw std::runtime_error(msg);
  }
}

void copy_data(struct archive *ar, struct archive *aw) {
  int r;
  const void *buff;
  size_t size;
  int64_t offset;

  for (;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return;
    handle_archive_error(ar, r);

    r = archive_write_data_block(aw, buff, size, offset);
    handle_archive_error(aw, r);
  }
}

std::filesystem::path
get_tar_output_path(const std::filesystem::path &target_path,
                    const std::filesystem::path &original_path,
                    const std::filesystem::path &tarball_name) {
  const auto leading_it = original_path.begin();
  if (leading_it == original_path.end()) {
    Diagnostics::error("Tarball contained an empty path");
    return target_path;
  }
  if (*leading_it != tarball_name) {
    Diagnostics::warn(
        "Tarball content is not contained within a directory of the same name");
    return target_path / original_path;
  }
  std::filesystem::path dest_path = target_path;
  for (auto it = std::next(leading_it); it != original_path.end(); ++it) {
    dest_path /= *it;
  }
  return dest_path;
}
} // namespace

void extract_tar(const std::filesystem::path &tar,
                 const std::filesystem::path &target_path) {
  struct archive *a;
  struct archive *ext;
  struct archive_entry *entry;
  int flags;
  int r;

  /* Select which attributes we want to restore. */
  flags = ARCHIVE_EXTRACT_TIME;
  flags |= ARCHIVE_EXTRACT_PERM;
  flags |= ARCHIVE_EXTRACT_ACL;
  flags |= ARCHIVE_EXTRACT_FFLAGS;

  // Open read
  a = archive_read_new();
  archive_read_support_format_all(a);
  archive_read_support_filter_all(a);
  if ((r = archive_read_open_filename(a, tar.generic_string().c_str(),
                                      10240))) {
    throw std::runtime_error("Error opening tarball for read");
  }

  // Assume tarballs have two extensions
  std::filesystem::path tarball_name = tar.stem().stem();

  // Open write
  ext = archive_write_disk_new();
  archive_write_disk_set_options(ext, flags);
  archive_write_disk_set_standard_lookup(ext);
  for (;;) {
    // Read next file
    r = archive_read_next_header(a, &entry);
    if (r == ARCHIVE_EOF)
      break;
    handle_archive_error(a, r);

    // Fix up destination path
    std::filesystem::path original_path{archive_entry_pathname(entry)};
    std::filesystem::path dest_path =
        get_tar_output_path(target_path, original_path, tarball_name);
    archive_entry_set_pathname(entry, dest_path.generic_string().c_str());

    // Write next file
    r = archive_write_header(ext, entry);
    handle_archive_error(ext, r);
    if (archive_entry_size(entry) > 0) {
      copy_data(a, ext);
    }

    // Finish writing file
    r = archive_write_finish_entry(ext);
    handle_archive_error(ext, r);
  }
  archive_read_close(a);
  archive_read_free(a);
  archive_write_close(ext);
  archive_write_free(ext);
}
} // namespace cimple
