#include "parser.h"

#include <iostream>

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

std::unique_ptr<ExpressionNode> Parser::parse_binary_expression(
    std::unique_ptr<ExpressionNode> lhs) {
  auto op = peek().type;
  advance();
  auto rhs = parse_float_expression();

  return std::make_unique<BinaryExpression>(op, std::move(lhs), std::move(rhs));
}

std::unique_ptr<ExpressionNode> Parser::parse_top_level() {
  auto lhs = parse_float_expression();
  switch (peek().type) {
    case TokenType::Asterisk:
    case TokenType::Minus:
    case TokenType::Plus:
    case TokenType::ForwardSlash:
      return parse_binary_expression(std::move(lhs));
    default:
      throw std::runtime_error("Expected binary expression");
  }
}
