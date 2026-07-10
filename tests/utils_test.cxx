#include <gtest/gtest.h>

#include <stdexcept>
#include <string_view>

#include "utils/container.h"

constexpr auto MAP = to_static_map<std::string_view, int32_t>({
    {"two", 2},
    {"one", 1},
});

TEST(StaticMapTest, FindGetsValidValueTest) {
  auto has_one = MAP.find("one");
  ASSERT_TRUE(has_one.has_value());
  ASSERT_EQ(*has_one, 1);
}

TEST(StaticMapTest, FindHandlesInvalidValueTest) {
  auto has_three = MAP.find("three");
  ASSERT_FALSE(has_three.has_value());
}

TEST(StaticMapTest, ThrowsOnDuplicateKeyTest) {
  ASSERT_THROW(([] -> void {
                 auto _ = to_static_map<std::string_view, int32_t>(
                     {{"one", 1}, {"one", 1}});
               }()),
               std::logic_error);
}

constexpr auto SET = to_static_set<int32_t>({1, 8, 6, 4, 2});

TEST(StaticSetTest, FindGetsValidValueTest) {
  auto has_one = SET.find(1);
  ASSERT_TRUE(has_one.has_value());
  ASSERT_EQ(*has_one, 1);
}

TEST(StaticSetTest, FindHandlesInvalidValueTest) {
  auto has_three = SET.find(10);
  ASSERT_FALSE(has_three.has_value());
}

TEST(StaticSetTest, ThrowsOnDuplicateValueTest) {
  ASSERT_THROW(([] -> void { auto _ = to_static_set<int32_t>({1, 2, 1}); }()),
               std::logic_error);
}
