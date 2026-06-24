#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <array>
#include <string>
#include <unordered_map>

enum class TokenType : int8_t {
  Plus,
  Minus,
  Asterisk,
  ForwardSlash,
  FloatLiteral,
  IntLiteral,
  StringLiteral,
  Identifier,
  LParen,
  RParen,
  Assignment,
  Colon,
  LessThan,
  GreaterThan,
  LessThanEquals,
  GreaterThanEquals,
  Equals,
  NotEquals,
  Not,

  // Keywords
  Set,
  Show,
  Function,
  End,
  Return,
  // EOF
  EndOfFile,
};

const std::unordered_map<std::string, TokenType> keywords = {
    {"set", TokenType::Set},       {"show", TokenType::Show},
    {"func", TokenType::Function}, {"end", TokenType::End},
    {"return", TokenType::Return},
};

const std::unordered_map<std::string, TokenType> symbols = {
    {"+", TokenType::Plus},
    {"-", TokenType::Minus},
    {"*", TokenType::Asterisk},
    {"/", TokenType::ForwardSlash},
    {"(", TokenType::LParen},
    {")", TokenType::RParen},
    {"=", TokenType::Assignment},
    {":", TokenType::Colon},
    {"<", TokenType::LessThan},
    {">", TokenType::GreaterThan},
    {"<=", TokenType::LessThanEquals},
    {">=", TokenType::GreaterThanEquals},
    {"==", TokenType::Equals},
    {"!=", TokenType::NotEquals},
    {"!", TokenType::Not},
};

constexpr std::array<const char*, 25> TOKEN_TYPE_STR = {
    "Plus",
    "Minus",
    "Asterisk",
    "ForwardSlash",
    "FloatLiteral",
    "IntLiteral",
    "StringLiteral",
    "Identifier",
    "LParen",
    "RParen",
    "Assignment",
    "Colon",
    "LessThan",
    "GreaterThan",
    "LessThanEquals",
    "GreaterThanEquals",
    "Equals",
    "NotEquals",
    "Not",
    "Set",
    "Show",
    "Function",
    "End",
    "Return",
    "EndOfFile",
};

inline const char* token_type_to_str(TokenType t) {
  auto index = static_cast<size_t>(t);
  if (index >= TOKEN_TYPE_STR.size()) return "Unknown";
  return TOKEN_TYPE_STR[index];
}

#endif  // TOKEN_TYPE_H
