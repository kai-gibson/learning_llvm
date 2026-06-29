#include <CLI/CLI.hpp>

#include "frontend.h"

int main(int argc, char** argv) {
  CLI::App app{"B2 compiler"};
  argv = app.ensure_utf8(argv);

  std::string filename;
  app.add_option("<filename>", filename, "Input file")->required();

  bool output_llvm{};
  app.add_flag("--output-llvm", output_llvm, "Output LLVM IR");

  CLI11_PARSE(app, argc, argv);

  compile(filename, output_llvm);
}
