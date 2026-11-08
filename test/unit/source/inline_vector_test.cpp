#include <alex_types/inline_vector.hpp>
#include <gtest/gtest.h>

using namespace alex;

TEST(inline_vector, simple_info) {
  constexpr const auto s = 128u;
  auto v = inline_vector<s, int>();
  EXPECT_EQ(v.max_length(), s);
  EXPECT_EQ(v.length(), 0);
  EXPECT_EQ(v.empty(), true);
  EXPECT_EQ(v.full(), false);
}

TEST(inline_vector, put_returns_valid_ptr) {
  auto vector = inline_vector<10, int>();
  auto p = vector.put(15);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 15);
}

TEST(inline_vector, put_increments_length) {
  auto v = inline_vector<10, int>();
  EXPECT_EQ(v.length(), 0);
  v.put(15);
  EXPECT_EQ(v.length(), 1);
  v.put(7);
  EXPECT_EQ(v.length(), 2);
  v.put(30);
  EXPECT_EQ(v.length(), 3);
}

TEST(inline_vector, put_beyond_max_returns_nullptr) {
  auto v = inline_vector<3, int>();
  auto p = v.put(1);
  EXPECT_NE(p, nullptr);
  p = v.put(2);
  EXPECT_NE(p, nullptr);
  p = v.put(3);
  EXPECT_NE(p, nullptr);
  p = v.put(4);
  EXPECT_EQ(p, nullptr);
}

TEST(inline_vector, peek_indexes_correctly) {
  auto v = inline_vector<int{4}, char>();
  v.put('a');
  v.put('b');
  v.put('c');
  v.put('d');
  ASSERT_EQ(v.length(), v.max_length());

  auto p = v.peek(0);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'a');

  p = v.peek(1);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'b');

  p = v.peek(2);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'c');

  p = v.peek(3);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'd');

  p = v.peek(4);
  ASSERT_EQ(p, nullptr);
}

TEST(inline_vector, negative_peek_indexes_correctly) {
  auto v = inline_vector<std::int32_t{4}, char>();
  v.put('a');
  v.put('b');
  v.put('c');
  v.put('d');
  ASSERT_EQ(v.length(), v.max_length());

  auto p = v.peek(-1);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'd');

  p = v.peek(-2);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'c');

  p = v.peek(-3);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'b');

  p = v.peek(-4);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'a');

  p = v.peek(-5);
  ASSERT_EQ(p, nullptr);
}

TEST(inline_vector, negative_peek_returns_nullptr_for_unsigned_length) {
  auto v = inline_vector<std::uint32_t{4}, char>();
  v.put('a');
  v.put('b');
  v.put('c');
  v.put('d');
  ASSERT_EQ(v.length(), v.max_length());

  auto p = v.peek(-1);
  ASSERT_EQ(p, nullptr);

  p = v.peek(-2);
  ASSERT_EQ(p, nullptr);

  p = v.peek(-3);
  ASSERT_EQ(p, nullptr);

  p = v.peek(-4);
  ASSERT_EQ(p, nullptr);

  p = v.peek(-5);
  ASSERT_EQ(p, nullptr);
}

TEST(inline_vector, last) {
  auto v = inline_vector<8, char>();
  EXPECT_EQ(v.last(), nullptr);

  v.put('a');
  auto p = v.last();
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'a');

  v.put('b');
  p = v.last();
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'b');
}
