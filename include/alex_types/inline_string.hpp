#pragma once

#include <cstring>
#include <format>
#include <string_view>

namespace alex {

using inline_string_length_t = std::size_t;

template <inline_string_length_t buffer_size> class inline_string {
public:
  static constexpr const auto buffer_size_v =
      (buffer_size < 1) ? 1 : buffer_size;

  static constexpr const auto max_writeable_size_v = buffer_size_v - 1;

  using character_t = char;
  using length_t = inline_string_length_t;

  template <length_t literal_size>
  using string_literal_t = const char (&)[literal_size];

  explicit constexpr inline_string(string_literal_t<buffer_size_v> str) {
    for (length_t i = 0; i < buffer_size_v; ++i) {
      _buffer[i] = str[i];
    }
  }

  template <length_t other_literal_size>
  explicit constexpr inline_string(string_literal_t<other_literal_size> str) {
    static_assert(other_literal_size <= buffer_size_v);
    for (length_t i = 0; i < other_literal_size; ++i) {
      _buffer[i] = str[i];
    }
  }

  constexpr auto try_set(std::string_view const other) -> bool {
    const auto characters_to_write = std::min(other.size(), buffer_size_v);
    if (characters_to_write > max_writeable_size_v) {
      return false;
    }

    std::strncpy(_buffer, other.data(), characters_to_write);
    _buffer[characters_to_write + 1] = '\0';
    return true;
  }

  constexpr auto max_writeable_size() const -> length_t { return buffer_size_v - 1; }

  constexpr auto size() const -> length_t {
    for (auto i = length_t{0}; i < buffer_size_v; i++) {
      if (_buffer[i] == '\0') {
        return i;
      }
    }

    return buffer_size_v;
  }

  constexpr auto view() const -> std::string_view { return {_buffer, size()}; }
  constexpr auto unsafe_dataptr() const -> const character_t * {
    return _buffer;
  }
  constexpr auto unsafe_mutable_dataptr() -> character_t * { return _buffer; }

  template <length_t other_size>
  constexpr auto operator==(inline_string<other_size> other) const -> bool {
    return std::strncmp(unsafe_dataptr(), other.unsafe_dataptr(),
                        std::min(other_size, size())) == 0;
  }

  template <length_t other_literal_size>
  constexpr auto operator==(string_literal_t<other_literal_size> other) const
      -> bool {
    return *this == inline_string<other_literal_size>(other);
  }

  template <length_t other_size_value>
  constexpr auto operator+(const inline_string<other_size_value> &other) const
      -> auto {
    constexpr const auto this_writeable_size = buffer_size_v - 1;
    constexpr const auto other_size = other_size_value;
    auto result = inline_string<this_writeable_size + other_size>("");

    auto index = length_t{0};
    for (length_t i = 0; i < this_writeable_size; ++i) {
      result.unsafe_mutable_dataptr()[index++] = unsafe_dataptr()[i];
    }

    for (length_t i = 0; i < other_size; ++i) {
      result.unsafe_mutable_dataptr()[index++] = other.unsafe_dataptr()[i];
    }

    return result;
  }

  template <length_t other_literal_size>
  constexpr auto operator+(string_literal_t<other_literal_size> other) const
      -> auto {
    return *this + inline_string<other_literal_size>(other);
  }

private:
  character_t _buffer[buffer_size_v]{};
};

template <inline_string_length_t size_value>
inline_string(const char (&)[size_value]) -> inline_string<size_value>;

} // namespace alex

namespace std {
template <size_t inline_string_size>
struct formatter<alex::inline_string<inline_string_size>> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  auto format(const alex::inline_string<inline_string_size> &obj,
              format_context &ctx) const {
    return format_to(ctx.out(), "{}", obj.view());
  }
};
} // namespace std
