#include "lexer.h"

#include <cctype>

char Lexer::current() { return index < data.size() ? data[index] : '\0'; }

char Lexer::peek() { return index + 1 < data.size() ? data[index + 1] : '\0'; }

Token Lexer::parse_number() {
  size_t start = index;
  while (std::isdigit(current())) advance();

  if (current() == '.' && std::isdigit(peek())) {
    advance();
    while (std::isdigit(current())) advance();
  }

  return {TokenType::FloatLiteral, data.substr(start, index - start)};
}

bool Lexer::is_identifier_char(char chr) {
  return std::isalnum(chr) || chr == '_' || chr == '-';
}

Token Lexer::parse_identifier() {
  size_t start = index;
  while (is_identifier_char(current())) advance();
  auto word = data.substr(start, index - start);

  auto it = keywords.find(word);
  if (it != keywords.end()) return {it->second, word};

  return {TokenType::Identifier, word};
}

Token Lexer::next_token() {
  while (index < data.size() && std::isspace(current())) advance();

  if (index >= data.size() || current() == '\0') {
    return {TokenType::EndOfFile, ""};
  }

  switch (current()) {
    case '+':
      advance();
      return {TokenType::Plus, "+"};
    case '-':
      advance();
      return {TokenType::Minus, "-"};
    case '*':
      advance();
      return {TokenType::Asterisk, "*"};
    case '/':
      advance();
      return {TokenType::ForwardSlash, "/"};
    case '(':
      advance();
      return {TokenType::LParen, "("};
    case ')':
      advance();
      return {TokenType::RParen, ")"};
    case '=':
      advance();
      return {TokenType::Assignment, "="};
    case ':':
      advance();
      return {TokenType::Colon, ":"};

    default:
      if (std::isdigit(current())) return parse_number();
      if (is_identifier_char(current())) return parse_identifier();

      advance();
      return {TokenType::EndOfFile, ""};
  }
}

Tokens Lexer::tokenise() {
  Tokens tokens;

  Token token = next_token();

  while (token.type != TokenType::EndOfFile) {
    tokens.push_back(token);
    token = next_token();
  }

  tokens.push_back(token);
  return tokens;
}
