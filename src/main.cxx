#include <CLI/CLI.hpp>

#include "frontend.h"

auto main(int argc, char** argv) -> int {
  try {
    CLI::App app{"B2 compiler"};
    argv = app.ensure_utf8(argv);

    std::string filename;
    app.add_option("<filename>", filename, "Input file")->required();

    bool output_llvm{};
    app.add_flag("--output-llvm", output_llvm, "Output LLVM IR");

    CLI11_PARSE(app, argc, argv);

    compile(filename, output_llvm);
  } catch (const std::exception& e) {
    std::cout << e.what();
  }
}
