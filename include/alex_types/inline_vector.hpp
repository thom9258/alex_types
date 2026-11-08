#pragma once

#include <concepts>
#include <type_traits>

#include <optional>

namespace alex {

namespace detail {
template <std::integral T, typename UT = std::make_unsigned_t<T>>
constexpr auto abs(T v) -> UT {
  if (v < 0) {
    return UT{0} - static_cast<UT>(v);
  }

  return static_cast<UT>(v);
}
} // namespace detail

template <std::integral auto N, typename T> class inline_vector {
public:
  using length_t = decltype(N);
  using unsigned_length_t = std::make_unsigned_t<length_t>;
  using index_t = length_t;
  static const constexpr auto max_length_v =
      static_cast<length_t>((N < 1) ? 1 : N);

  static const constexpr bool allows_negative_indexing_v =
      std::signed_integral<length_t>;

  using value_t = T;
  using reference_t = T &;
  using const_reference_t = const T &;
  using pointer_t = T *;
  using const_pointer_t = const T *;

  constexpr auto max_length() const -> length_t { return max_length_v; }

  constexpr auto length() const -> length_t { return _length; }

  constexpr auto unsigned_length() const -> unsigned_length_t {
    return static_cast<unsigned_length_t>(_length);
  }

  constexpr auto unsafe_dataptr() const -> const_pointer_t { return _buffer; }

  constexpr auto unsafe_mutable_dataptr() -> pointer_t { return _buffer; }

  constexpr auto full() const -> bool { return length() == max_length(); }

  constexpr auto empty() const -> bool { return length() == 0; }

  constexpr auto put(const_reference_t value) -> pointer_t {
    if (full()) {
      return nullptr;
    }

    _buffer[_length] = value;
    const auto p = &_buffer[_length];
    _length++;
    return p;
  }

  constexpr auto last() -> pointer_t {
    if (empty()) {
      return nullptr;
    }

    return &_buffer[_length - 1];
  }

  constexpr auto last() const -> const_pointer_t { return last(); }

  constexpr auto pop() -> std::optional<value_t> {
    auto target = last();
    if (target == nullptr) {
      return std::nullopt;
    }

    auto popped = std::move(*target);
    _length--;
    return popped;
  }

  constexpr auto peek(index_t index) -> pointer_t {
    if (empty()) {
      return nullptr;
    }

    if constexpr (allows_negative_indexing_v) {
      if (index >= length()) {
        return nullptr;
      }

      if (index < 0 && detail::abs(index) > unsigned_length()) {
        return nullptr;
      }

      if (index < 0) {
        return &_buffer[length() + index];
      }

      return &_buffer[index];

    } else {
      if (index >= length()) {
        return nullptr;
      }

      return &_buffer[index];
    }
  }

  constexpr auto peek(index_t index) const -> const_pointer_t {
    return peek(index);
  }

private:
  value_t _buffer[max_length_v];
  length_t _length{0};
};

} // namespace alex
