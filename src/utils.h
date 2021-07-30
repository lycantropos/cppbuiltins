#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>

using undefined = void;

template <class T>
struct double_precision {
  using type = undefined;
};

template <class T>
constexpr T power(const T base, const std::size_t exponent) {
  return exponent == 0 ? T(1) : T(base) * power<T>(base, exponent - 1);
}

constexpr std::size_t floor_log10(const std::size_t value) {
  return value < 10 ? 0 : floor_log10(value / 10) + 1;
}

std::size_t floor_log2(std::size_t value) {
  std::size_t result = 0;
  for (; value; ++result, value >>= 1)
    ;
  return result - 1;
}

#endif
