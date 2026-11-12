#pragma once

#include <cstddef>
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

  constexpr ~inline_vector() { reset(); }

  constexpr auto max_length() const -> length_t { return max_length_v; }

  constexpr auto length() const -> length_t { return _length; }

  constexpr auto full() const -> bool { return length() == max_length(); }

  constexpr auto empty() const -> bool { return length() == 0; }

  [[nodiscard]]
  constexpr auto put(const_reference_t value) & -> pointer_t {
    if (full()) {
      return nullptr;
    }

    std::construct_at(&_buffer[_length], value);
    _length++;
    return last();
  }

  constexpr auto put(const_reference_t value) && = delete;

  template <typename... Args>
  [[nodiscard]]
  constexpr auto emplace(Args &&...args) & -> pointer_t {
    if (full()) {
      return nullptr;
    }

    std::construct_at(&_buffer[_length], std::forward<Args>(args)...);
    _length++;
    return last();
  }

  template <typename... Args>
  constexpr auto emplace(Args &&...args) && = delete;

  template <typename... Args>
  [[nodiscard]]
  constexpr auto emplace_at(index_t index, Args &&...args) & -> pointer_t {
    if (full() || !in_range(index)) {
      return nullptr;
    }

    auto value = value_t(std::forward<Args>(args)...);
    // move the last element back to the end, this ensures last element does not
    // call operator= with a uninitialized this*
    std::construct_at(&_buffer[_length], std::move(_buffer[_length - 1]));
    _length++;

    std::move_backward(_buffer[index], _buffer[_length - 2],
                       _buffer[_length - 1]);
    _buffer[index] = std::move(value);
    return &_buffer[index];
  }

  constexpr auto last() & -> pointer_t {
    return empty() ? nullptr : &_buffer[_length - 1];
  }

  constexpr auto last() const & -> const_pointer_t {
    return empty() ? nullptr : &_buffer[_length - 1];
  }

  constexpr auto last() const && = delete;

  constexpr auto first() & -> pointer_t {
    return empty() ? nullptr : &_buffer[0];
  }

  constexpr auto first() const & -> const_pointer_t {
    return empty() ? nullptr : &_buffer[0];
  }

  constexpr auto first() const && = delete;

  constexpr auto begin() const & -> const_pointer_t { return _buffer; }
  constexpr auto begin() const && = delete;

  constexpr auto end() const & -> const_pointer_t { return &_buffer[_length]; }
  constexpr auto end() const && = delete;

  constexpr auto unsafe_begin() & -> pointer_t { return _buffer; }
  constexpr auto unsafe_begin() && = delete;

  constexpr auto unsafe_end() & -> pointer_t { return &_buffer[_length]; }
  constexpr auto unsafe_end() && = delete;

  constexpr auto peek(index_t index) & -> pointer_t {
    return in_range(index) ? &_buffer[index] : nullptr;
  }

  constexpr auto peek(index_t index) const & -> const_pointer_t {
    return in_range(index) ? &_buffer[index] : nullptr;
  }

  constexpr auto peek(index_t index) const && = delete;

  constexpr auto pop() -> void {
    if (empty()) {
      return;
    }

    std::destroy_at(last());
    _length--;
  }

  constexpr auto reset() -> void {
    while (!empty()) {
      std::destroy_at(last());
      _length--;
    }
  }

private:
  constexpr auto in_range(index_t index) const -> bool {
    return index < length();
  }

  union { // Defer construction of value_t
    value_t _buffer[max_length_v];
  };

  length_t _length{0};
};

} // namespace alex

static_assert(alex::inline_vector<0, char>::max_length_v == 1);
