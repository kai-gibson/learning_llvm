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

  // Keywords
  Set,
  Show,
  Function,
  End,
  Return,
  And,
  Or,
  If,
  Else,
  Not,
  // EOF
  EndOfFile,
};

constexpr auto TOKEN_TYPE_STR = std::to_array({
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
    "Set",
    "Show",
    "Function",
    "End",
    "Return",
    "And",
    "Or",
    "If",
    "Else",
    "Not",
    "EndOfFile",
});

const std::unordered_map<std::string, TokenType> keywords = {
    {"set", TokenType::Set},       {"show", TokenType::Show},
    {"func", TokenType::Function}, {"end", TokenType::End},
    {"return", TokenType::Return}, {"and", TokenType::And},
    {"or", TokenType::Or},         {"if", TokenType::If},
    {"else", TokenType::Else},     {"not", TokenType::Not},
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
};

inline const char* token_type_to_str(TokenType t) {
  auto index = static_cast<size_t>(t);
  if (index >= TOKEN_TYPE_STR.size()) return "Unknown";
  return TOKEN_TYPE_STR[index];
}

#endif  // TOKEN_TYPE_H
