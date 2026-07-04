#include "lexer.h"

#include <gtest/gtest.h>

#include <algorithm>

TEST(IsIdentifierCharTest, WorksForAllIdentifiersTest) {
  for (char chr = '!'; chr <= '~'; chr++) {
    bool expected = std::isalnum(chr) || chr == '_' || chr == '-';
    EXPECT_EQ(is_identifier_char(chr), expected)
        << "Failed for char: '" << chr << "' (0x" << std::hex << (int)chr
        << ")";
  }
}

constexpr auto VALID_SYMBOLS = std::to_array({
    '+',
    '-',
    '*',
    '/',
    '(',
    ')',
    '=',
    ':',
    '<',
    '>',
    '{',
    '}',
});

TEST(IsSymbolCharTest, FailsOnInvalidSymbols) {
  for (char chr = '!'; chr <= '~'; chr++) {
    using std::ranges::contains;

    if (!contains(VALID_SYMBOLS, chr)) {
      EXPECT_EQ(is_symbol_char(chr), false)
          << "Failed for char: '" << chr << "' (0x" << std::hex << (int)chr
          << ")";
    }
  }
}

TEST(IsSymbolCharTest, WorksForAllValidSymbolsTest) {
  for (const auto chr : VALID_SYMBOLS) {
    bool expected =
        std::isprint(chr) && !std::isalnum(chr) && !std::isspace(chr);

    EXPECT_EQ(is_symbol_char(chr), expected)
        << "Failed for char: '" << chr << "' (0x" << std::hex << (int)chr
        << ")";
  }
}
