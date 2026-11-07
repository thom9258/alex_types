#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

namespace alex {

namespace detail {

template <typename type_t, typename... args_t>
  requires(std::is_same_v<type_t, args_t> || ...)
[[nodiscard]]
consteval auto index_of() -> std::size_t {
  constexpr bool matches[]{std::is_same_v<type_t, args_t>..., false};
  for (std::size_t index{0}; index < sizeof...(args_t); ++index) {
    if (matches[index]) {
      return index;
    }
  }

  std::unreachable();
}

template <typename first_t, typename... rest_t> struct union_chain {
  union choice_t {
    constexpr choice_t() {}
    constexpr ~choice_t() {}
    first_t value;
    union_chain<rest_t...> next;
  } choice;
};

template <typename last_t> struct union_chain<last_t> {
  union choice_t {
    constexpr choice_t() {}
    constexpr ~choice_t() {}
    last_t value;
  } choice;
};

template <std::size_t index_v, typename... args_t>
    constexpr auto at(union_chain<args_t...> *link) -> args_t... [index_v] *
{
  if constexpr (index_v == 0) {
    return std::addressof(link->choice.value);
  } else {
    return at<index_v - 1>(std::addressof(link->choice.next));
  }
}

    template <class... Fs>
    struct type_matcher : Fs... {
  using Fs::operator()...;
};

} // namespace detail

inline constexpr const auto tagged_union_ntag =
  std::numeric_limits<std::size_t>::max();

template <typename... args_t> class tagged_union {
public:
  template <typename type_t>
  // requires(detail::contains<type_t, args_t...>())
    requires(std::is_same_v<type_t, args_t> || ...)
  static consteval auto tag_of() -> std::size_t {
    return detail::index_of<type_t, args_t...>();
  }

  template <typename type_t> constexpr auto set(const type_t &value) -> void {
    constexpr auto tag = tag_of<type_t>();
    auto *target = detail::at<tag>(&_chain);
    if (target != nullptr) {
      *target = value;
      _tag = tag;
    }
  }

  template <typename type_t> constexpr auto set(type_t &&value) -> void {
    constexpr auto tag = tag_of<type_t>();
    auto *target = detail::at<tag>(&_chain);
    if (target != nullptr) {
      *target = std::move(value);
      _tag = tag;
    }
  }

  constexpr auto tag() const -> std::size_t { return _tag; }

  template <std::size_t tag_v> constexpr auto is() const -> bool {
    return _tag == tag_v;
  }

  template <typename type_t> constexpr auto is() const -> bool {
    return tag_of<type_t>() == _tag;
  }

  template <std::size_t tag_v>
      constexpr auto peek() -> args_t... [tag_v] *
  {
    if (!is<tag_v>()) {
      return nullptr;
    }

    return detail::at<tag_v>(&_chain);
  }

      template <typename type_t>
      constexpr auto peek() -> type_t * {
    return peek<tag_of<type_t>()>();
  }

  template <typename visitor_t>
  constexpr decltype(auto) visit(visitor_t &&vis) {
    using result_t = std::invoke_result_t<visitor_t &, args_t...[0] &>;
    static_assert(
        (std::is_same_v<result_t,
                        std::invoke_result_t<visitor_t &, args_t &>> &&
         ...),
        "all visitor overloads must return the same type");

    return [&]<std::size_t... is>(std::index_sequence<is...>) -> result_t {
      using fn_t = result_t (*)(tagged_union &, visitor_t &);
      static constexpr fn_t table[] = {
          [](tagged_union &self, visitor_t &v) -> result_t {
            return std::invoke(v, *detail::at<is>(&self._chain));
          }...};
      return table[_tag](*this, vis);
    }(std::index_sequence_for<args_t...>{});
  }

  template <typename result_t, typename... fs_t>
  constexpr auto match(fs_t &&...fs) -> result_t {
    return visit<result_t>(detail::type_matcher{std::forward<fs_t>(fs)...});
  }

  template <typename... fs_t> constexpr auto match(fs_t &&...fs) {
    return visit(detail::type_matcher{std::forward<fs_t>(fs)...});
  }

private:
  std::size_t _tag{tagged_union_ntag};
  detail::union_chain<args_t...> _chain;
};

} // namespace alex
