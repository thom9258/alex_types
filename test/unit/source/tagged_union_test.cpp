#include <alex_types/tagged_union.hpp>
#include <gtest/gtest.h>

#include <type_traits>
#include <print>

using namespace alex;

struct vec2 {
  float x, y;
};

using simple_tagged_union = tagged_union<int, double, vec2>;

static_assert(sizeof(simple_tagged_union) ==
              sizeof(std::size_t) + sizeof(vec2));

static_assert(std::is_default_constructible_v<simple_tagged_union>);
static_assert(std::is_destructible_v<simple_tagged_union>);

static_assert(simple_tagged_union::tag_of<int>() == 0);
static_assert(simple_tagged_union::tag_of<double>() == 1);
static_assert(simple_tagged_union::tag_of<vec2>() == 2);

TEST(tagged_union, default_initialized_is_ntag) {
  auto v = simple_tagged_union{};
  EXPECT_EQ(v.tag(), tagged_union_ntag);
}

TEST(tagged_union, default_initialized_is_none) {
  auto v = simple_tagged_union{};
  EXPECT_EQ(v.is<tagged_union_ntag>(), true);
  EXPECT_EQ(v.is<0>(), false);
  EXPECT_EQ(v.is<1>(), false);
  EXPECT_EQ(v.is<2>(), false);
}

TEST(tagged_union, default_initialized_cant_peek) {
  auto v = simple_tagged_union{};
  EXPECT_EQ(v.peek<int>(), nullptr);
  EXPECT_EQ(v.peek<double>(), nullptr);
  EXPECT_EQ(v.peek<vec2>(), nullptr);

  EXPECT_EQ(v.peek<0>(), nullptr);
  EXPECT_EQ(v.peek<1>(), nullptr);
  EXPECT_EQ(v.peek<2>(), nullptr);
}

TEST(tagged_union, construct_correctly_sets_internal_state) {
  auto v = simple_tagged_union{};

  v.set(27);
  EXPECT_TRUE(v.is<int>());
  EXPECT_EQ(v.tag(), 0);

  v.set(3.13);
  EXPECT_TRUE(v.is<double>());
  EXPECT_EQ(v.tag(), 1);

  v.set(vec2{1.01f, 0.5f});
  EXPECT_TRUE(v.is<vec2>());
  EXPECT_EQ(v.tag(), 2);
}

TEST(tagged_union, construct) {
  auto v = simple_tagged_union{};

  v.set(vec2{1.01f, 0.5f});

  switch (v.tag()) {
  case v.tag_of<vec2>():
    std::println("is vec2");
    break;
  case v.tag_of<double>():
    std::println("is double");
    break;
  case v.tag_of<int>():
    std::println("is int");
    break;
  };

  if (int *p = v.peek<int>()) {
    std::println("modified int");
    *p = 2;
  } else if (double *p = v.peek<1>()) {
    std::println("modified double");
    *p = 3.14;
  } else if (vec2 *p = v.peek<vec2>()) {
    std::println("modified vec2");
    p->x = 4;
  }

  const auto stringified =
      v.match([](int i) { return std::format("int {}", i); },
              [](double d) { return std::format("double {}", d); },
              [](vec2 v) { return std::format("vec2 {},{}", v.x, v.y); });
  std::println("value: {}", stringified);

  const auto is_vec2 =
      v.match([](auto) { return false; }, [](vec2) { return true; });
  std::println("was vec2: {}", is_vec2 ? "yes" : "no");
}



// Check that the tagged union does not care about destructors or constructors
struct destruct_signal {
  destruct_signal(bool *target) : _target{target} {}

  destruct_signal(const destruct_signal &other) = delete;

  destruct_signal &operator=(const destruct_signal &) = delete;

  destruct_signal(destruct_signal &&other) {
    std::swap(_target, other._target);
  }

  destruct_signal &operator=(destruct_signal &&other) {
    std::swap(_target, other._target);
    return *this;
  }

  ~destruct_signal() {
    if (_target) {
      *_target = true;
    }
  }

  bool *_target{nullptr};
};

using tagged_union_destructable = tagged_union<double, vec2, destruct_signal>;
static_assert(std::is_default_constructible_v<tagged_union_destructable>);

TEST(tagged_union, destruct_signal_works) {
  auto did_destruct = false;
  {
    auto signal = destruct_signal(&did_destruct);
    EXPECT_FALSE(did_destruct);
  }

  EXPECT_TRUE(did_destruct);
}

TEST(tagged_union, union_does_not_call_destructors) {
  auto did_destruct = false;
  {
    auto v = tagged_union<int, destruct_signal>{};
    v.set(destruct_signal(&did_destruct));
    EXPECT_FALSE(did_destruct);
  }

  EXPECT_FALSE(did_destruct);
}
