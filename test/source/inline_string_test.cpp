#include <alex_types/inline_string.hpp>
#include <gtest/gtest.h>

using namespace alex;

static_assert(sizeof(inline_string<10>) ==
              sizeof(inline_string<1>::character_t) * 10);

static_assert(sizeof(inline_string("helloworld")) == sizeof("helloworld"));

TEST(inline_string, construct_from_literal) {
  auto v = inline_string{"my string literal"};
  EXPECT_GE(v.size(), 0u);
}

TEST(inline_string, construct_with_smaller_literal) {
  inline_string<64> v("oke");
  EXPECT_EQ(v.size(), 3u);
  EXPECT_EQ(v.max_writeable_size(), 63u);
  EXPECT_EQ(sizeof(v), sizeof(decltype(v)::character_t) * 64u);
}

TEST(inline_string, comparison) {
  EXPECT_EQ(inline_string("hello"), "hello");
  EXPECT_EQ(inline_string("jackie"), inline_string("jackie"));
  EXPECT_NE(inline_string("bob"), "alice");
  EXPECT_NE(inline_string("alice"), inline_string("carter"));
  EXPECT_NE(inline_string("foo"), inline_string("bar"));
}

TEST(inline_string, addition) {
  const auto added = inline_string("foo") + inline_string("barbaz");
  EXPECT_EQ(added.view(), "foobarbaz");
}

TEST(inline_string, formattable) {
  const auto formatted = std::format("{}", inline_string("mystr"));
  EXPECT_EQ(formatted, "mystr");
}

inline constexpr const auto base_dir = inline_string("/users/bob");
inline constexpr const auto images_dir = base_dir + "/images";
inline constexpr const auto backgrounds_dir = images_dir + "/backgrounds";
inline constexpr const auto documents_dir = base_dir + "/documents";
inline constexpr const auto homework_dir = documents_dir + "/homework";

TEST(inline_string, works_as_composable_globals) {
  EXPECT_EQ(base_dir, "/users/bob");
  EXPECT_EQ(images_dir, "/users/bob/images");
  EXPECT_EQ(backgrounds_dir, "/users/bob/images/backgrounds");
  EXPECT_EQ(documents_dir, "/users/bob/documents");
  EXPECT_EQ(homework_dir, "/users/bob/documents/homework");
}

TEST(inline_string, try_set) {
  auto buffer = inline_string<124>("");
  ASSERT_TRUE(buffer.try_set("jeff"));
  EXPECT_EQ(buffer, "jeff");
  ASSERT_TRUE(buffer.try_set("hello"));
  EXPECT_EQ(buffer.view(), "hello");
  ASSERT_TRUE(buffer.try_set("space cowboy"));
  EXPECT_EQ(buffer.view(), "space cowboy");

  const auto exact_fit = std::string(buffer.max_writeable_size(), 'a');
  ASSERT_TRUE(buffer.try_set(exact_fit));

  const auto too_large = std::string(buffer.max_writeable_size() + 1, 'a');
  ASSERT_FALSE(buffer.try_set(too_large));
}
