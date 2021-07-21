#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>

template <class T>
struct double_precision {
  using type = T;
};

template <>
struct double_precision<std::int8_t> {
  using type = std::int16_t;
};

template <>
struct double_precision<std::int16_t> {
  using type = std::int32_t;
};

template <>
struct double_precision<std::int32_t> {
  using type = std::int64_t;
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

template <std::size_t N>
constexpr std::size_t power_of_ten() {
  return 10 * power_of_ten<N - 1>();
}

template <>
constexpr std::size_t power_of_ten<0>() {
  return 1;
}

constexpr std::size_t floor_log10(std::size_t value) {
  return value < 10 ? 0 : floor_log10(value / 10) + 1;
}

template <std::size_t N>
constexpr std::size_t floor_log10() {
  static_assert(N, "Argument should be positive.");
  return floor_log10<N / 10>() + 1;
}

template <>
constexpr std::size_t floor_log10<1>() {
  return 0;
}

template <>
constexpr std::size_t floor_log10<2>() {
  return 0;
}

template <>
constexpr std::size_t floor_log10<3>() {
  return 0;
}

template <>
constexpr std::size_t floor_log10<4>() {
  return 0;
}

template <>
constexpr std::size_t floor_log10<5>() {
  return 0;
}

template <>
constexpr std::size_t floor_log10<6>() {
  return 0;
}

template <>
constexpr std::size_t floor_log10<7>() {
  return 0;
}

template <>
constexpr std::size_t floor_log10<8>() {
  return 0;
}

template <>
constexpr std::size_t floor_log10<9>() {
  return 0;
}

std::size_t floor_log2(std::size_t value) {
  std::size_t result = 0;
  for (; value; ++result, value >>= 1)
    ;
  return result - 1;
}

#endif
