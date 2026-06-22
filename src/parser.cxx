#include "parser.h"

#include <iostream>

int32_t precedence(TokenType t) {
  switch (t) {
    case TokenType::Plus:
    case TokenType::Minus:
      return 1;
    case TokenType::Asterisk:
    case TokenType::ForwardSlash:
      return 2;
    default:
      return 0;
  }
}

const Token& Parser::consume(TokenType expected) {
  if (peek().type != expected) {
    throw std::runtime_error(std::format("Expected: {}, got: {}",
                                         token_type_to_str(expected),
                                         token_type_to_str(peek().type)));
  }

  auto& token = peek();
  advance();
  return token;
}

const Token& Parser::peek() {
  // return EOF token instead of OOB
  return (index < tokens.size()) ? tokens[index] : tokens.back();
}

void Parser::advance() { index += 1; }

std::unique_ptr<ExpressionNode> Parser::parse_float_expression() {
  auto token = peek();
  std::clog << "converting: " << token.value << " to double...\n";
  auto value = std::stod(token.value);
  auto result = std::make_unique<FloatLiteralExpression>(value);
  advance();

  return std::move(result);
}

std::unique_ptr<ExpressionNode> Parser::parse_expression(
    int32_t min_precedence) {
  auto lhs = parse_float_expression();

  while (precedence(peek().type) > min_precedence) {
    auto op = peek().type;
    advance();
    auto rhs = parse_expression(precedence(op));
    lhs =
        std::make_unique<BinaryExpression>(op, std::move(lhs), std::move(rhs));
  }

  return lhs;
}

std::unique_ptr<ExpressionNode> Parser::parse_top_level() {
  return parse_expression(0);
}
