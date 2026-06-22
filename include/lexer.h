// lexer.h
#ifndef LEXER_H
#define LEXER_H

#include <array>
#include <cctype>
#include <cstdint>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

enum class TokenType : int8_t {
  Plus,
  Minus,
  Asterisk,
  ForwardSlash,
  FloatLiteral,
  Identifier,
  LParen,
  RParen,
  Assignment,
  Set,
  Show,
  Function,
  End,
  EndOfFile,
};

const std::unordered_map<std::string, TokenType> keywords = {
    {"set", TokenType::Set},
    {"show", TokenType::Show},
    {"func", TokenType::Function},
    {"end", TokenType::End},
};

constexpr std::array<const char*, 14> TOKEN_TYPE_STR = {
    "Plus",       "Minus",    "Asterisk", "ForwardSlash", "FloatLiteral",
    "Identifier", "LParen",   "RParen",   "Assignment",   "Set",
    "Show",       "Function", "End",      "EndOfFile",
};

inline const char* token_type_to_str(TokenType t) {
  auto index = static_cast<size_t>(t);
  if (index >= TOKEN_TYPE_STR.size()) return "Unknown";
  return TOKEN_TYPE_STR[index];
}

struct Token {
  TokenType type;
  std::string value;

  Token(TokenType type, std::string value)
      : type(type), value(std::move(value)) {}
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
  const std::string& data;
  size_t index{};

  void advance() { index += 1; }
  char current();
  char peek();

  Token parse_number();
  Token parse_identifier();
  Token next_token();
  bool is_identifier_char(char chr);
};

#endif  // LEXER_H
