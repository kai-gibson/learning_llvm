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

  // Function call expression
  if (peek().type == TokenType::LParen) {
    // No args implemented yet
    advance();
    consume(TokenType::RParen);
    return std::make_unique<FunctionCallExpression>(name);
  }

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
      throw std::runtime_error(std::format("Unexpected primary expression: {}",
                                           token_type_to_str(peek().type)));
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

std::unique_ptr<ASTNode> Parser::parse_type_expression() {
  auto [_, name] = consume(TokenType::Identifier);

  if (!is_builtin_type(name))
    throw std::runtime_error(std::format("Unknown type: {}", name));

  return std::make_unique<TypeExpression>(name);
}

std::unique_ptr<ASTNode> Parser::parse_variable_declaration() {
  auto name = peek().value;
  advance();

  std::unique_ptr<ASTNode> type_identifier = nullptr;
  // explicitly typed variable
  if (peek().type == TokenType::Colon) {
    advance();

    type_identifier = parse_type_expression();
  }

  consume(TokenType::Assignment);
  return std::make_unique<VariableDeclarationStatement>(
      name, parse_expression(0), std::move(type_identifier));
};

std::unique_ptr<ASTNode> Parser::parse_variable_assignment() {
  consume(TokenType::Set);
  auto [_, name] = consume(TokenType::Identifier);
  consume(TokenType::Assignment);

  return std::make_unique<VariableAssignmentStatement>(name,
                                                       parse_expression(0));
}

std::unique_ptr<ASTNode> Parser::parse_show_statement() {
  consume(TokenType::Show);

  return std::make_unique<ShowStatement>(parse_expression(0));
}

std::unique_ptr<ASTNode> Parser::parse_statement() {
  switch (peek().type) {
    case TokenType::Identifier:
      return parse_variable_declaration();
    case TokenType::Set:
      return parse_variable_assignment();
    case TokenType::Show:
      return parse_show_statement();
    case TokenType::Return:
      return parse_return_statement();
    default:
      throw std::runtime_error("Unexpected statement");
  }
};

std::unique_ptr<ASTNode> Parser::parse_function_declaration() {
  consume(TokenType::Function);
  auto [_, name] = consume(TokenType::Identifier);

  auto func = std::make_unique<FunctionDeclaration>(name);

  consume(TokenType::LParen);
  consume(TokenType::RParen);

  while (peek().type != TokenType::End) {
    func->statements.push_back(parse_statement());
  }

  consume(TokenType::End);

  return std::move(func);
}

std::unique_ptr<ASTNode> Parser::parse_top_level() {
  auto program = std::make_unique<Program>();

  while (peek().type != TokenType::EndOfFile) {
    program->functions.push_back(parse_function_declaration());
  }

  return std::move(program);
}

std::unique_ptr<ASTNode> Parser::parse_return_statement() {
  consume(TokenType::Return);
  return std::make_unique<ReturnStatement>(parse_expression(0));
}
