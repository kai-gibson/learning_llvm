#ifndef PARSER_H
#define PARSER_H

#include <algorithm>
#include <array>
#include <string_view>

#include "ast.h"
#include "lexer.h"

constexpr bool is_builtin_type(std::string_view target) {
  constexpr std::array builtin_types = {
      "Int8",   "Int16",  "Int32",   "Int64",   "UInt8",  "UInt16",
      "UInt32", "UInt64", "Float32", "Float64", "String", "Bool"};

  return std::ranges::any_of(builtin_types, [target](std::string_view item) {
    return item == target;
  });
}

class Parser {
 public:
  Parser(const Tokens& tokens) : tokens(tokens) {}
  std::unique_ptr<ASTNode> parse_top_level();

 private:
  void advance();
  const Token& peek();
  const Token& consume(TokenType expected);

  std::unique_ptr<ASTNode> parse_primary_expression();
  std::unique_ptr<ASTNode> parse_paren_expression();
  std::unique_ptr<ASTNode> parse_float_expression();
  std::unique_ptr<ASTNode> parse_identifier_expression();
  std::unique_ptr<ASTNode> parse_binary_expression(std::unique_ptr<ASTNode> lhs,
                                                   int32_t min_precedence);
  std::unique_ptr<ASTNode> parse_expression(int32_t min_precedence);
  std::unique_ptr<ASTNode> parse_statement();
  std::unique_ptr<ASTNode> parse_variable_declaration();
  std::unique_ptr<ASTNode> parse_variable_assignment();
  std::unique_ptr<ASTNode> parse_show_statement();
  std::unique_ptr<ASTNode> parse_function_declaration();
  std::unique_ptr<ASTNode> parse_return_statement();
  std::unique_ptr<ASTNode> parse_type_expression();

  // vars
  const Tokens& tokens;
  std::size_t index = 0;
};

#endif  // PARSER_H
