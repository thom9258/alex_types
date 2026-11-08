#include <alex_types/inline_lookuptable.hpp>
#include <gtest/gtest.h>

#include <array>

using namespace alex;

TEST(binary_search, invalid_input_returns_nullptr) {
  EXPECT_EQ(nullptr, detail::binary_search<char>(7, nullptr, 0));
  EXPECT_EQ(nullptr, detail::binary_search<char>(7, nullptr, 5));
  auto arr = std::array<detail::entry<char>, 2>{};
  EXPECT_EQ(nullptr, detail::binary_search<char>(7, arr.data(), 0));
}

TEST(binary_search, can_find_different_valid_entries) {
  auto arr = std::array{detail::entry<char>{15, 'a'}, //
                        detail::entry<char>{77, 'b'}, //
                        detail::entry<char>{132, 'c'}};

  auto kv = detail::binary_search<char>(132, arr.data(), arr.size());
  ASSERT_NE(kv, nullptr);
  EXPECT_EQ(kv->key, 132);
  EXPECT_EQ(kv->value, 'c');

  kv = detail::binary_search<char>(15, arr.data(), arr.size());
  ASSERT_NE(kv, nullptr);
  EXPECT_EQ(kv->key, 15);
  EXPECT_EQ(kv->value, 'a');

  kv = detail::binary_search<char>(77, arr.data(), arr.size());
  ASSERT_NE(kv, nullptr);
  EXPECT_EQ(kv->key, 77);
  EXPECT_EQ(kv->value, 'b');
}

TEST(binary_search, can_not_find_invalid_entries) {
  auto arr =
      std::array{detail::entry<char>{3, 'a'}, detail::entry<char>{5, 'b'}};

  auto kv = detail::binary_search<char>(7, arr.data(), arr.size());
  EXPECT_EQ(kv, nullptr);

  kv = detail::binary_search<char>(0, arr.data(), arr.size());
  EXPECT_EQ(kv, nullptr);

  kv = detail::binary_search<char>(4, arr.data(), arr.size());
  EXPECT_EQ(kv, nullptr);
}
