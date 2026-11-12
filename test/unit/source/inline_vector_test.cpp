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

TEST(inline_vector, emplace_increments_length) {
  auto v = inline_vector<10, int>();
  EXPECT_EQ(v.length(), 0);
  v.emplace(15);
  EXPECT_EQ(v.length(), 1);
  v.emplace(7);
  EXPECT_EQ(v.length(), 2);
  v.emplace(30);
  EXPECT_EQ(v.length(), 3);
}

struct non_default_constructable {
  explicit non_default_constructable(int v) : _v{v} {}
  int _v;
};

TEST(inline_vector, emplace_works_for_non_default_constructable_types) {
  auto v = inline_vector<10, non_default_constructable>();
  v.emplace(3);
  v.emplace(4);
  EXPECT_EQ(v.length(), 2);
}

struct as_mutable {
  static auto &view(auto &v) { return v; }
};
struct as_const {
  static auto &view(auto &v) { return std::as_const(v); }
};

template <typename Access>
class inline_vector_access : public ::testing::Test {};

using access_kinds = ::testing::Types<as_mutable, as_const>;
TYPED_TEST_SUITE(inline_vector_access, access_kinds);

TYPED_TEST(inline_vector_access, peek_indexes_correctly) {
  auto v = inline_vector<int{4}, char>();
  v.put('a');
  v.put('b');
  v.put('c');
  v.put('d');

  auto& view = TypeParam::view(v);
  ASSERT_EQ(view.length(), view.max_length());

  auto p = view.peek(0);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'a');

  p = view.peek(1);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'b');

  p = view.peek(2);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'c');

  p = view.peek(3);
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'd');

  p = view.peek(4);
  ASSERT_EQ(p, nullptr);
}

TYPED_TEST(inline_vector_access, last) {
  auto v = inline_vector<8, char>();
  auto& view = TypeParam::view(v);

  EXPECT_EQ(view.last(), nullptr);

  v.put('a');
  auto p = view.last();
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'a');

  v.put('b');
  p = view.last();
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'b');
}

TYPED_TEST(inline_vector_access, first) {
  auto v = inline_vector<8, char>();
  auto& view = TypeParam::view(v);

  EXPECT_EQ(view.first(), nullptr);

  v.put('a');
  auto p = view.first();
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'a');

  v.put('b');
  p = view.first();
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(*p, 'a');
}
