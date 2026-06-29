#include "frontend.h"

#include "codegen/code_generation_visitor.h"
#include "file_io.h"
#include "lexer.h"
#include "parser.h"
#include "print_visitor.h"
#include "type_check_visitor.h"

void compile(const std::string& filename, bool output_llvm) {
  auto file_data = read_entire_file(filename);
  if (!file_data.has_value()) {
    std::cerr << "Error reading file: " << file_data.error().what() << '\n';
  }

  Lexer lexer(FileContents{.name = filename, .data = *file_data});
  auto tokens = lexer.tokenise();

  Parser parser(tokens);

  auto top = parser.parse_top_level();
  TypeCheckVisitor type_checker;

  top->accept(type_checker);

  PrintVisitor v;
  top->accept(v);

  CodegenVisitor codegen;
  top->accept(codegen);

  if (output_llvm) codegen.output_llvm(filename);

  codegen.compile();
}
