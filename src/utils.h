#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>

using undefined = void;

template <class T>
struct double_precision {
  using type = undefined;
};

template <>
struct double_precision<std::uint8_t> {
  using type = std::uint16_t;
};

template <>
struct double_precision<std::uint16_t> {
  using type = std::uint32_t;
};

template <>
struct double_precision<std::uint32_t> {
  using type = std::uint64_t;
};

template <class T>
using double_precision_t = typename double_precision<T>::type;

template <class T>
T euclidean_algorithm(T first, T second) {
  while (second != 0) {
    const T step = second;
    second = first % second;
    first = step;
  }
  return first;
}

template <std::size_t BASE>
constexpr std::size_t floor_log(const std::size_t value) {
  return value < BASE ? 0 : floor_log<BASE>(value / BASE) + 1;
}

std::size_t floor_log2(std::size_t value) {
  std::size_t result = 0;
  for (; value; ++result, value >>= 1)
    ;
  return result - 1;
}

template <class T>
constexpr T power(const T base, const std::size_t exponent) {
  return exponent == 0 ? T(1) : T(base) * power<T>(base, exponent - 1);
}

static constexpr std::size_t bit_lengths_table[32] = {
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

template <class T>
constexpr std::size_t to_bit_length(const T value) {
  std::size_t result = 0;
  T step = value;
  while (step >= 32) {
    result += 6;
    step >>= 6;
  }
  result += bit_lengths_table[step];
  return result;
}

#endif
