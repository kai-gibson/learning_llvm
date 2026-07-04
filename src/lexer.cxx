#include "lexer.h"

#include <cctype>

// free pure functions
bool is_identifier_char(char chr) {
  return std::isalnum(chr) || chr == '_' || chr == '-';
}

bool is_symbol_char(char chr) {
  switch (chr) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '(':
    case ')':
    case '=':
    case ':':
    case '<':
    case '>':
    case '{':
    case '}':
      return true;
    default:
      return false;
  }
}

char Lexer::current() { return index < data.size() ? data[index] : '\0'; }

char Lexer::peek() { return index + 1 < data.size() ? data[index + 1] : '\0'; }

Token Lexer::parse_number() {
  size_t start = index;

  auto token_type = TokenType::IntLiteral;

  while (std::isdigit(current())) advance();

  if (current() == '.' && std::isdigit(peek())) {
    token_type = TokenType::FloatLiteral;
    advance();
    while (std::isdigit(current())) advance();
  }

  return {token_type, data.substr(start, index - start), token_location};
}

Token Lexer::parse_identifier() {
  size_t start = index;
  while (is_identifier_char(current())) advance();
  auto word = data.substr(start, index - start);

  auto it = keywords.find(word);
  if (it != keywords.end()) return {it->second, word, token_location};

  return {TokenType::Identifier, word, token_location};
}

Token Lexer::next_token() {
  while (index < data.size() && std::isspace(current())) advance();

  if (index >= data.size() || current() == '\0') {
    return {TokenType::EndOfFile, "", token_location};
  }

  token_location = current_location;

  if (is_symbol_char(current())) return parse_symbol();
  if (std::isdigit(current())) return parse_number();
  if (is_identifier_char(current())) return parse_identifier();

  current_location = token_location;
  advance();
  return {TokenType::EndOfFile, "", token_location};
}

Token Lexer::parse_symbol() {
  if (current() == '(') {
    advance();
    return {TokenType::LParen, "(", token_location};
  }
  if (current() == ')') {
    advance();
    return {TokenType::RParen, ")", token_location};
  }

  size_t start = index;
  while (is_symbol_char(current())) advance();
  auto word = data.substr(start, index - start);

  auto it = symbols.find(word);
  if (it != symbols.end()) return {it->second, word, token_location};

  throw std::runtime_error(std::format("Error: Unknown token: {}", word));
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

void Lexer::advance() {
  if (current() == '\n') {
    current_location.column = 1;
    current_location.line += 1;
  } else {
    current_location.column += 1;
  }

  index += 1;
}
