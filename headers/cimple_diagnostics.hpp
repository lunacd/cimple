#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace cimple {
class Diagnostics {
public:
  enum class OutputMode {
    Store,
    Console,
  };

  enum class DiagnosticType { Info, Warning, Error };

  class Message {
  public:
    std::string message;
    DiagnosticType type;
  };

  static void diagnostics(std::string_view message, DiagnosticType type);
  static void info(std::string_view message) {
    Diagnostics::diagnostics(message, DiagnosticType::Info);
  }
  static void warn(std::string_view message) {
    Diagnostics::diagnostics(message, DiagnosticType::Warning);
  }
  static void error(std::string_view message) {
    Diagnostics::diagnostics(message, DiagnosticType::Error);
  }
  static void configure_output_mode(OutputMode mode);
  static std::vector<Message> history;

private:
  static OutputMode m_output_mode;
};
} // namespace cimple
