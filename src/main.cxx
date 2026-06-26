#include <format>
#include <fstream>
#include <iostream>

#include "codegen/code_generation_visitor.h"
#include "file_io.h"
#include "lexer.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "parser.h"
#include "print_visitor.h"
#include "type_check_visitor.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: ./learning_llvm <file.b>";
    return EXIT_FAILURE;
  }

  const auto filename = std::string(argv[1]);
  auto file_data = read_entire_file(filename);
  if (!file_data.has_value()) {
    std::cerr << "Error reading file: " << file_data.error().what() << '\n';
  }

  Lexer lexer(FileContents{.name = filename, .data = *file_data});
  auto tokens = lexer.tokenise();
  // std::cout << "tokens: " << tokens << "\n";

  Parser parser(tokens);

  auto top = parser.parse_top_level();
  TypeCheckVisitor type_checker;

  top->accept(type_checker);

  PrintVisitor v;
  top->accept(v);

  CodegenVisitor codegen;
  top->accept(codegen);
  codegen.finalise();

  auto outfile = std::format("{}.ll", filename);
  std::ofstream ofs(outfile);

  codegen.llvm_module->print(llvm::errs(), nullptr);

  std::error_code EC;
  llvm::raw_fd_ostream file(outfile, EC, llvm::sys::fs::OF_None);

  if (!EC) {
    codegen.llvm_module->print(file, nullptr);
    file.close();
  } else {
    llvm::errs() << "Error opening file: " << EC.message() << "\n";
  }

  codegen.compile();
}
