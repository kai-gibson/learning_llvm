#include <gtest/gtest.h>

#include "codegen/code_generation_visitor.h"
#include "lexer.h"
#include "parser.h"
#include "type_check_visitor.h"

auto codegen(std::string_view program) -> CodegenVisitor {
  std::string program_text = R"(
  func main()
    return 1
  end
  )";

  auto tokens =
      Lexer(FileContents{.name = "test.b", .data = program_text}).tokenise();
  auto root = Parser(tokens).parse_top_level();

  auto type_checker = TypeCheckVisitor();
  root->accept(type_checker);

  CodegenVisitor codegen;
  root->accept(codegen);
  return codegen;
};

TEST(CodegenTest, GeneratesSimpleFunction) {
  auto program = R"(
  func main()
    return 1
  end
  )";

  auto gen = codegen(program);
  auto func = gen.llvm_module->getFunction("main");
  ASSERT_TRUE(func->getReturnType()->isIntegerTy(32));
}

TEST(CodegenTest, GeneratesVariableDeclaration) {
  auto program = R"(
    func main()
      x = 10

      return x
    end
  )";

  auto gen = codegen(program);
  auto func = gen.llvm_module->getFunction("main");
  ASSERT_TRUE(func->getReturnType()->isIntegerTy(32));

  for (const auto& instruction : func->getEntryBlock()) {
    std::cout << instruction.getName().data() << "\n";
  }
}
