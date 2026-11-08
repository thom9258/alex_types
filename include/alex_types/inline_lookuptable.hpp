#pragma once

#include <alex_types/inline_vector.hpp>

#include <cstddef>

namespace alex {

using lookuptable_key_t = std::size_t;

namespace detail {

template <typename T> struct entry {
  lookuptable_key_t key;
  T value;
};

template <typename T>
constexpr auto binary_search(lookuptable_key_t target, entry<T> *entries, std::size_t count)
    -> entry<T> * {
  if (entries == nullptr || count == 0) {
    return nullptr;
  }

  auto begin = std::size_t{0};
  auto end = std::size_t{count};
  while (begin < end) {
    const auto mid = begin + (end - begin) / 2;
    const auto key = entries[mid].key;
    if (key == target) {
      return &entries[mid];
    } else if (key > target) {
      end = mid;
    } else {
      begin = mid + 1;
    }
  }

  return nullptr;
}

} // namespace detail


/**
 * lookup is O(log(n))
 * entries are unique, sorted and stored inline
 */
template <std::size_t N, typename T> class inline_lookuptable {
public:
  using length_t = decltype(N);
  static const constexpr auto max_length_v =
      static_cast<length_t>((N < 1) ? 1 : N);

  using key_t = lookuptable_key_t;
  using value_t = T;
  using reference_t = T &;
  using const_reference_t = const T &;
  using rvalue_reference_t = T &&;
  using pointer_t = T *;
  using const_pointer_t = const T *;

  constexpr auto max_length() const -> length_t { return _table.max_length(); }

  constexpr auto length() const -> length_t { return _table.length(); }

  constexpr auto full() const -> bool { return _table.full(); }

  constexpr auto empty() const -> bool { return _table.empty(); }

  constexpr auto insert(key_t key, const_reference_t value) -> void;

  constexpr auto insert(key_t key, rvalue_reference_t value) -> void;

  constexpr auto find(key_t key) -> pointer_t;

  constexpr auto find(key_t key) const -> const_pointer_t { return find(key); }

  constexpr auto contains(key_t key) const -> bool {
    return find(key) != nullptr;
  }

  constexpr auto reset() -> void { _table.reset(); }

private:
  inline_vector<N, detail::entry<value_t>> _table;
};

} // namespace alex
