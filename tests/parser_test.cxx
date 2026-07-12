#include "parser.h"

#include <gtest/gtest.h>

#include "lexer.h"

auto parse_simple_expression(const std::string& body)
    -> std::unique_ptr<ASTNode> {
  std::string program = "func main() " + body + " return 0 end";

  auto tokens =
      Lexer(FileContents{.name = "test.b", .data = program}).tokenise();
  auto parser = Parser(tokens);

  auto node = parser.parse_top_level();

  auto prog = dynamic_cast<Program*>(node.get());
  if (!prog) throw std::runtime_error("program not found");

  auto fn = dynamic_cast<FunctionDeclaration*>(prog->functions.at(0).get());
  if (!fn) throw std::runtime_error("fn not found");

  return std::move(fn->statements.at(0));
}

auto parse_statements(const std::string& body)
    -> std::vector<std::unique_ptr<ASTNode>> {
  std::string program = "func main() " + body + " return 0 end";

  auto tokens =
      Lexer(FileContents{.name = "test.b", .data = program}).tokenise();
  auto parser = Parser(tokens);

  auto node = parser.parse_top_level();

  auto prog = dynamic_cast<Program*>(node.get());
  if (!prog) throw std::runtime_error("program not found");

  auto fn = dynamic_cast<FunctionDeclaration*>(prog->functions.at(0).get());
  if (!fn) throw std::runtime_error("fn not found");

  fn->statements.pop_back();  // remove return statement

  return std::move(fn->statements);
}

template <class T>
auto cast_node(std::unique_ptr<ASTNode>& node) -> T* {
  auto ptr = dynamic_cast<T*>(node.get());
  if (!ptr)
    throw std::runtime_error(
        std::format("Failed to cast node to type: {}", typeid(T).name()));
  return ptr;
}

TEST(ParserTest, ParsesIntLiteralVarAssignment) {
  auto root = parse_simple_expression("x = 1234");
  auto decl = cast_node<VariableDeclarationStatement>(root);

  auto value = cast_node<IntLiteralExpression>(decl->value);

  ASSERT_EQ(value->value, 1234);
  EXPECT_EQ(decl->name, "x");
}

TEST(ParserTest, ParsesFloatLiteralVarAssignment) {
  auto root = parse_simple_expression("x = 35.131");
  auto decl = cast_node<VariableDeclarationStatement>(root);

  auto value = cast_node<FloatLiteralExpression>(decl->value);

  ASSERT_EQ(value->value, 35.131);
  EXPECT_EQ(decl->name, "x");
}

TEST(ParserTest, ParsesIntAdditionVarAssignment) {
  auto root = parse_simple_expression("x = 123 + 321");
  auto decl = cast_node<VariableDeclarationStatement>(root);

  auto value = cast_node<BinaryExpression>(decl->value);

  ASSERT_EQ(value->op, TokenType::Plus);

  auto left = cast_node<IntLiteralExpression>(value->lhs);
  auto right = cast_node<IntLiteralExpression>(value->rhs);

  ASSERT_EQ(left->value, 123);
  EXPECT_EQ(right->value, 321);
}

TEST(ParserTest, ParsesFloatAdditionVarAssignment) {
  auto root = parse_simple_expression("x = 1.23 + 2.34");
  auto decl = cast_node<VariableDeclarationStatement>(root);

  auto value = cast_node<BinaryExpression>(decl->value);
  ASSERT_EQ(value->op, TokenType::Plus);

  auto left = cast_node<FloatLiteralExpression>(value->lhs);
  auto right = cast_node<FloatLiteralExpression>(value->rhs);

  ASSERT_EQ(left->value, 1.23);
  EXPECT_EQ(right->value, 2.34);
}

TEST(ParserTest, ParsesShowStatement) {
  auto root = parse_simple_expression("show 123");
  auto show = cast_node<ShowStatement>(root);

  auto value = cast_node<IntLiteralExpression>(show->expr);

  ASSERT_EQ(value->value, 123);
}

TEST(ParserTest, ParsesSetStatement) {
  auto root = parse_statements("x = 10\nset x = 12");
  auto show = cast_node<VariableAssignmentStatement>(root.at(1));

  auto value = cast_node<IntLiteralExpression>(show->value);

  ASSERT_EQ(show->name, "x");
  EXPECT_EQ(value->value, 12);
}

TEST(ParserTest, ParsesMultiplication) {
  auto expr = parse_simple_expression("x = 10 * 20");
  auto assign = cast_node<VariableDeclarationStatement>(expr);

  auto mult = cast_node<BinaryExpression>(assign->value);
  ASSERT_EQ(mult->op, TokenType::Asterisk);
  auto left = cast_node<IntLiteralExpression>(mult->lhs);
  auto right = cast_node<IntLiteralExpression>(mult->rhs);

  ASSERT_EQ(assign->name, "x");
  EXPECT_EQ(left->value, 10);
  EXPECT_EQ(right->value, 20);
}

TEST(ParserTest, ParsesDivision) {
  auto expr = parse_simple_expression("x = 10 / 20");
  auto assign = cast_node<VariableDeclarationStatement>(expr);

  auto div = cast_node<BinaryExpression>(assign->value);
  ASSERT_EQ(div->op, TokenType::ForwardSlash);
  auto left = cast_node<IntLiteralExpression>(div->lhs);
  auto right = cast_node<IntLiteralExpression>(div->rhs);

  ASSERT_EQ(assign->name, "x");
  EXPECT_EQ(left->value, 10);
  EXPECT_EQ(right->value, 20);
}

TEST(ParserTest, ParsesVariableIncrement) {
  auto stmts = parse_statements("x = 12\nset x = x * 2");
  ASSERT_EQ(stmts.size(), 2);

  auto declare = cast_node<VariableDeclarationStatement>(stmts.at(0));

  ASSERT_EQ(declare->name, "x");
  auto value = cast_node<IntLiteralExpression>(declare->value)->value;

  ASSERT_EQ(value, 12);

  auto assign = cast_node<VariableAssignmentStatement>(stmts.at(1));
  auto add = cast_node<BinaryExpression>(assign->value);

  auto left = cast_node<VariableExpression>(add->lhs);
  auto right = cast_node<IntLiteralExpression>(add->rhs);

  ASSERT_EQ(left->name, "x");
  ASSERT_EQ(right->value, 2);
}
