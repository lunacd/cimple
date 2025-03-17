#include <cimple_diagnostics.hpp>

#include <iostream>

namespace cimple {
void Diagnostics::diagnostics(std::string_view message, DiagnosticType type) {
  switch (Diagnostics::m_output_mode) {
  case OutputMode::Console:
    switch (type) {
    case DiagnosticType::Info:
      std::cout << message << "\n";
      break;
    case DiagnosticType::Warning:
      std::cerr << "Warning: " << message << "\n";
      break;
    case DiagnosticType::Error:
      std::cerr << "Error: " << message << "\n";
      break;
    }
    break;
  case OutputMode::Store:
    Diagnostics::history.emplace_back(std::string(message), type);
  }
}

void Diagnostics::configure_output_mode(OutputMode mode) {
  Diagnostics::m_output_mode = mode;
}

std::vector<Diagnostics::Message> Diagnostics::history = {};

Diagnostics::OutputMode Diagnostics::m_output_mode =
    Diagnostics::OutputMode::Console;
} // namespace cimple
