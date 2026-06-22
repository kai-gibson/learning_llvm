#include "parser.h"

#include <iostream>

int32_t precedence(TokenType t) {
  switch (t) {
    case TokenType::Assignment:
      return 1;
    case TokenType::Plus:
    case TokenType::Minus:
      return 2;
    case TokenType::Asterisk:
    case TokenType::ForwardSlash:
      return 3;
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

std::unique_ptr<ASTNode> Parser::parse_float_expression() {
  auto token = peek();
  std::clog << "converting: " << token.value << " to double...\n";
  auto value = std::stod(token.value);
  auto result = std::make_unique<FloatLiteralExpression>(value);
  advance();

  return std::move(result);
}

std::unique_ptr<ASTNode> Parser::parse_paren_expression() {
  advance();
  auto expr = parse_expression(0);
  if (peek().type != TokenType::RParen) {
    throw std::runtime_error("Expected ')'");
  }

  advance();
  return expr;
}

std::unique_ptr<ASTNode> Parser::parse_identifier_expression() {
  std::string name = peek().value;
  advance();

  return std::make_unique<VariableExpression>(name);
}

std::unique_ptr<ASTNode> Parser::parse_primary_expression() {
  switch (peek().type) {
    case TokenType::FloatLiteral:
      return parse_float_expression();
    case TokenType::LParen:
      return parse_paren_expression();
    case TokenType::Identifier:
      return parse_identifier_expression();
    default:
      throw std::runtime_error("Unexpected primary expression");
  }
}

std::unique_ptr<ASTNode> Parser::parse_expression(int32_t min_precedence) {
  auto lhs = parse_primary_expression();

  while (precedence(peek().type) > min_precedence) {
    auto op = peek().type;
    advance();
    auto rhs = parse_expression(precedence(op));
    lhs =
        std::make_unique<BinaryExpression>(op, std::move(lhs), std::move(rhs));
  }

  return lhs;
}

std::unique_ptr<ASTNode> Parser::parse_top_level() {
  auto program = std::make_unique<Program>();

  while (peek().type != TokenType::EndOfFile) {
    program->statements.push_back(parse_expression(0));
  }

  return std::move(program);
}
