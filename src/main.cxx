#include <iostream>

#include "code_generation_visitor.h"
#include "file_io.h"
#include "lexer.h"
#include "parser.h"
#include "print_visitor.h"

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

  Lexer lexer(*file_data);
  auto tokens = lexer.tokenise();
  std::cout << "tokens: " << tokens << "\n";

  Parser parser(tokens);

  auto top = parser.parse_top_level();
  PrintVisitor v;
  top->accept(v);

  CodegenVisitor codegen;
  top->accept(codegen);
  codegen.finalise();
  codegen.llvm_module->print(llvm::errs(), nullptr);
}
