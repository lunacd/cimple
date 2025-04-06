#include <cimple_cmd.hpp>

#include <filesystem>
#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
  po::options_description desc;
  desc.add_options()("command", po::value<std::string>(), "command to execute")(
      "subargs", po::value<std::vector<std::string>>(),
      "Arguments for command");

  po::positional_options_description positional_desc;
  positional_desc.add("command", 1).add("subargs", -1);
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(desc)
                .positional(positional_desc)
                .run(),
            vm);
  po::notify(vm);

  if (vm.count("command") == 0) {
    std::cout << "cimple, a simple C/C++ build tool\n";
    std::cout << "Usage: cimple [command]\n";
    std::cout << "Commands:\n";
    std::cout << "\tbuild: Build a cimple project\n";
    std::cout << "\tbuild-pkg: Build a cimple PI package\n";
    return 0;
  }

  std::string cmd = vm["command"].as<std::string>();
  if (cmd == "build") {
    cimple::cmd::build(std::filesystem::current_path());
  } else if (cmd == "build-pkg") {
    cimple::cmd::build_pkg(std::filesystem::current_path());
  }
}
