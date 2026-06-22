#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

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

  // vars
  const Tokens& tokens;
  std::size_t index = 0;
};

#endif  // PARSER_H
