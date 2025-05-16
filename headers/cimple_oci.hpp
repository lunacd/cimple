#include <chrono>
#include <string>
#include <vector>

#include <cpp-subprocess/subprocess.hpp>

namespace cimple::oci {
class OciContainerMount {
public:
  std::string type;
  std::string source;
  std::string destination;
};

class OciContainerStartConfig {
public:
  std::string image;
  std::vector<OciContainerMount> mounts;

  //! Maximum time this container can stay alive
  std::chrono::duration<int> timeout;

  std::vector<std::string> get_sleep_command();
};

// TODO: this needs to be templated to support multiple OCI implementations
class OciContainer {
public:
  OciContainer(OciContainerStartConfig &&config);
  ~OciContainer();

  subprocess::Popen run_command(const std::vector<std::string> &cmd);

private:
  std::vector<std::string> get_mount_options(const OciContainerStartConfig &config);

  std::string m_container_name;
};
} // namespace cimple::oci
