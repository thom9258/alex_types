#pragma once

#include <functional>

template <typename T, typename Tag> class strong_type {
public:
  using underlying_t = T;
  using reference_t = T&;
  using const_reference_t = const T&;
  using rvalue_reference_t = T&&;

  explicit constexpr strong_type(const_reference_t value) : _value{value} {}
  explicit constexpr strong_type(rvalue_reference_t value) : _value{std::move(value)} {}

  template <typename... Args>
  static constexpr auto create(Args &&...args) -> strong_type<T, Tag> {
    return strong_type<T, Tag>(underlying_t{std::forward<Args>(args)...});
  }

  constexpr auto value() -> reference_t { return _value; }
  constexpr auto value() const -> const_reference_t { return value(); }
  constexpr auto operator*() -> reference_t { return value(); }
  constexpr auto operator*()const -> const_reference_t { return value(); }

private:
  underlying_t _value;
};
