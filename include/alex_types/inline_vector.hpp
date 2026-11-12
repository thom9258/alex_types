#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace alex {

template <std::size_t N, typename T> class inline_vector {
public:
  using length_t = decltype(N);
  using index_t = length_t;
  static const constexpr auto max_length_v =
      static_cast<length_t>((N < 1) ? 1 : N);

  using value_t = T;
  using reference_t = T &;
  using const_reference_t = const T &;
  using pointer_t = T *;
  using const_pointer_t = const T *;

  constexpr inline_vector() {}

  constexpr ~inline_vector()
    requires std::is_trivially_destructible_v<value_t>
  = default;

  constexpr ~inline_vector() { std::destroy_n(_buffer, _length); }

  constexpr auto max_length() const -> length_t { return max_length_v; }

  constexpr auto length() const -> length_t { return _length; }

  constexpr auto unsafe_dataptr() const -> const_pointer_t { return _buffer; }

  constexpr auto unsafe_mutable_dataptr() -> pointer_t { return _buffer; }

  constexpr auto full() const -> bool { return length() == max_length(); }

  constexpr auto empty() const -> bool { return length() == 0; }

  constexpr auto put(const_reference_t value) -> pointer_t {
    if (full()) {
      return nullptr;
    }

    std::construct_at(&_buffer[_length], value);
    _length++;
    return last();
  }

  template <typename... Args>
  constexpr auto emplace(Args &&...args) -> pointer_t {
    if (full()) {
      return nullptr;
    }

    std::construct_at(&_buffer[_length], std::forward<Args>(args)...);
    _length++;
    return last();
  }

  constexpr auto last() -> pointer_t {
    return empty() ? nullptr : &_buffer[_length - 1];
  }

  constexpr auto last() const -> const_pointer_t {
    return empty() ? nullptr : &_buffer[_length - 1];
  }

  constexpr auto first() -> pointer_t {
    return empty() ? nullptr : &_buffer[0];
  }

  constexpr auto first() const -> const_pointer_t {
    return empty() ? nullptr : &_buffer[0];
  }

  constexpr auto end() const -> const_pointer_t { return &_buffer[_length]; }

  constexpr auto peek(index_t index) -> pointer_t {
    return in_range(index) ? &_buffer[index] : nullptr;
  }

  constexpr auto peek(index_t index) const -> const_pointer_t {
    return in_range(index) ? &_buffer[index] : nullptr;
  }

  constexpr auto pop() -> void {
    if (empty()) {
      return;
    }

    std::destroy_at(last());
    _length--;
  }

  constexpr auto reset() -> void {
    std::destroy_n(_buffer, _length);
    _length = 0;
  }

private:
  constexpr auto in_range(index_t index) const -> bool { return index < length(); }

  union { // Defer construction of value_t
    value_t _buffer[max_length_v];
  };

  length_t _length{0};
};

} // namespace alex
