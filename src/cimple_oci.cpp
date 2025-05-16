#include <cimple_diagnostics.hpp>
#include <cimple_oci.hpp>

#include <chrono>
#include <format>
#include <type_traits>

#include <cpp-subprocess/subprocess.hpp>
#include <uuid_v4.h>

namespace cimple::oci {
static_assert(std::is_aggregate_v<OciContainerStartConfig>,
              "OciContainerStartConfig should be an aggregate");

std::vector<std::string> OciContainerStartConfig::get_sleep_command() {
  int num_seconds = std::chrono::seconds(timeout).count();

#ifdef _WIN32
  return {"Start-Sleep", "-Seconds", std::to_string(num_seconds)};
#else
  return {"sleep", std::format("{}s", num_seconds)};
#endif
}

OciContainer::OciContainer(OciContainerStartConfig &&config) {
  // Generate UUID for container
  UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
  UUIDv4::UUID uuid = uuidGenerator.getUUID();
  std::string uuid_s = uuid.str();

  m_container_name = std::format("cimple-{}", uuid_s);
  const auto sleep_command = config.get_sleep_command();
  const auto mount_options = get_mount_options(config);

  std::vector<std::string> command = {
      "docker", "run",       "--name", m_container_name,
      "--rm",   "--network", "none",   "--detach",
  };

  command.insert(command.end(), mount_options.begin(), mount_options.end());

  command.emplace_back(config.image);

  command.insert(command.end(), sleep_command.begin(), sleep_command.end());

  // Start container
  auto p = subprocess::Popen(command);
  const auto rc = p.wait();
  if (rc != 0) {
    Diagnostics::error(std::format("Failed to start image {}", config.image));
  }
}

OciContainer::~OciContainer() {
  subprocess::Popen p{{"docker", "kill", m_container_name}};
  const auto rc = p.wait();
  if (rc != 0) {
    Diagnostics::error(
        std::format("Failed to stop container {}", m_container_name));
  }
}

subprocess::Popen
OciContainer::run_command(const std::vector<std::string> &cmd) {
  std::vector<std::string> exec_cmd{"docker", "exec", m_container_name};
  exec_cmd.insert(exec_cmd.end(), cmd.begin(), cmd.end());
  subprocess::Popen p{exec_cmd};
  return p;
}

std::vector<std::string>
OciContainer::get_mount_options(const OciContainerStartConfig &config) {
  std::vector<std::string> mount_options{};
  mount_options.reserve(config.mounts.size() * 2);
  for (const auto &mount : config.mounts) {
    mount_options.emplace_back("--mount");
    mount_options.emplace_back(std::format("type={},src={},dst={}", mount.type,
                                           mount.source, mount.destination));
  }
  return mount_options;
}

} // namespace cimple::oci
