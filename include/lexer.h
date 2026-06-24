// lexer.h
#ifndef LEXER_H
#define LEXER_H

#include <cctype>
#include <ostream>
#include <string>
#include <vector>

#include "source_location.h"
#include "token_type.h"

struct Token {
  TokenType type;
  std::string value;

  SourceLocation source_location;

  Token(TokenType type, std::string value, SourceLocation source_location)
      : type(type), value(std::move(value)), source_location(source_location) {}
};

inline std::ostream& operator<<(std::ostream& os, const Token& token) {
  return os << "Token{ .type=" << token_type_to_str(token.type) << " .value=\""
            << token.value << "\"}";
}

using Tokens = std::vector<Token>;

inline std::ostream& operator<<(std::ostream& os, const Tokens& tokens) {
  os << "[";
  for (size_t i = 0; i < tokens.size(); i++) {
    os << tokens.at(i);
    if (i != tokens.size() - 1) os << ",";
  }
  return os << "]";
}

class Lexer {
 public:
  Lexer(const std::string& file) : data(file) {}

  Tokens tokenise();

 private:
  SourceLocation current_location;
  const std::string& data;
  size_t index{};

  void advance();
  char current();
  char peek();

  Token parse_number();
  Token parse_identifier();
  Token parse_symbol();
  Token next_token();
  bool is_identifier_char(char chr);
  bool is_symbol_char(char chr);
};

#endif  // LEXER_H
