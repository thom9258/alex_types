#include <alex_types/inline_string.hpp>

#include <print>

int main() {
  auto string = alex::inline_string("hi, this is alex");
  std::println("inline string: {}", string);
}
