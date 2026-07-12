#include "parser.h"

#include <gtest/gtest.h>

#include "lexer.h"

using Nodes = std::vector<std::unique_ptr<ASTNode>>;
using Node = std::unique_ptr<ASTNode>;

template <class T>
auto cast_node(Node& node) -> T* {
  auto ptr = dynamic_cast<T*>(node.get());
  if (!ptr)
    throw std::runtime_error(
        std::format("Failed to cast node to type: {}", typeid(T).name()));
  return ptr;
}

auto parse_statements(const std::string& body, Nodes& nodes) -> void {
  std::string program = "func main() " + body + " return 0 end";

  auto tokens =
      Lexer(FileContents{.name = "test.b", .data = program}).tokenise();
  auto parser = Parser(tokens);

  auto node = parser.parse_top_level();

  auto prog = dynamic_cast<Program*>(node.get());
  ASSERT_NE(prog, nullptr) << "Program not generated in parse_statements";

  auto fn = dynamic_cast<FunctionDeclaration*>(prog->functions.at(0).get());
  ASSERT_NE(fn, nullptr) << "Function not generated in parse_statements";

  ASSERT_FALSE(fn->statements.empty()) << "No statements generated";
  auto ret = dynamic_cast<ReturnStatement*>(fn->statements.back().get());
  ASSERT_NE(ret, nullptr)
      << "Return statemnt not generated in parse_statements";

  fn->statements.pop_back();  // remove return statement

  nodes = std::move(fn->statements);
}

auto parse_simple_expression(const std::string& body, Node& node) -> void {
  Nodes nodes;
  ASSERT_NO_FATAL_FAILURE(parse_statements(body, nodes));

  ASSERT_GT(nodes.size(), 0) << "Failed to generate statements";
  ASSERT_EQ(nodes.size(), 1) << "Prefer parse_statements for > 1 statements";
  node = std::move(nodes.at(0));
}

TEST(ParserTest, ParsesIntLiteralVarAssignment) {
  Node root;
  ASSERT_NO_FATAL_FAILURE(parse_simple_expression("x = 1234", root));
  auto decl = cast_node<VariableDeclarationStatement>(root);

  auto value = cast_node<IntLiteralExpression>(decl->value);

  ASSERT_EQ(value->value, 1234);
  EXPECT_EQ(decl->name, "x");
}

TEST(ParserTest, ParsesFloatLiteralVarAssignment) {
  Node root;
  ASSERT_NO_FATAL_FAILURE(parse_simple_expression("x = 35.131", root));
  auto decl = cast_node<VariableDeclarationStatement>(root);

  auto value = cast_node<FloatLiteralExpression>(decl->value);

  ASSERT_EQ(value->value, 35.131);
  EXPECT_EQ(decl->name, "x");
}

TEST(ParserTest, ParsesIntAdditionVarAssignment) {
  Node root;
  ASSERT_NO_FATAL_FAILURE(parse_simple_expression("x = 123 + 321", root));

  auto decl = cast_node<VariableDeclarationStatement>(root);

  auto value = cast_node<BinaryExpression>(decl->value);

  ASSERT_EQ(value->op, TokenType::Plus);

  auto left = cast_node<IntLiteralExpression>(value->lhs);
  auto right = cast_node<IntLiteralExpression>(value->rhs);

  ASSERT_EQ(left->value, 123);
  EXPECT_EQ(right->value, 321);
}

TEST(ParserTest, ParsesFloatAdditionVarAssignment) {
  Node root;
  ASSERT_NO_FATAL_FAILURE(parse_simple_expression("x = 1.23 + 2.34", root));
  auto decl = cast_node<VariableDeclarationStatement>(root);

  auto value = cast_node<BinaryExpression>(decl->value);
  ASSERT_EQ(value->op, TokenType::Plus);

  auto left = cast_node<FloatLiteralExpression>(value->lhs);
  auto right = cast_node<FloatLiteralExpression>(value->rhs);

  ASSERT_EQ(left->value, 1.23);
  EXPECT_EQ(right->value, 2.34);
}

TEST(ParserTest, ParsesShowStatement) {
  Node root;
  ASSERT_NO_FATAL_FAILURE(parse_simple_expression("show 123", root));
  auto show = cast_node<ShowStatement>(root);

  auto value = cast_node<IntLiteralExpression>(show->expr);

  ASSERT_EQ(value->value, 123);
}

TEST(ParserTest, ParsesSetStatement) {
  Nodes stmts;
  ASSERT_NO_FATAL_FAILURE(parse_statements("x = 10\nset x = 12", stmts));
  ASSERT_EQ(stmts.size(), 2);

  auto show = cast_node<VariableAssignmentStatement>(stmts.at(1));
  auto value = cast_node<IntLiteralExpression>(show->value);

  ASSERT_EQ(show->name, "x");
  EXPECT_EQ(value->value, 12);
}

TEST(ParserTest, ParsesMultiplication) {
  Node expr;
  ASSERT_NO_FATAL_FAILURE(parse_simple_expression("x = 10 * 20", expr));
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
  Node expr;
  ASSERT_NO_FATAL_FAILURE(parse_simple_expression("x = 10 / 20", expr));

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
  Nodes stmts;
  ASSERT_NO_FATAL_FAILURE(parse_statements("x = 12\nset x = x * 2", stmts));
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
