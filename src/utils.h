#ifndef UTILS_HPP
#define UTILS_HPP

#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace cppbuiltins {
template <class T, class U>
struct is_upcastable
    : std::integral_constant<
          bool, std::is_arithmetic_v<T> && std::is_arithmetic_v<U> &&
                    std::is_integral_v<T> == std::is_integral_v<U> &&
                    std::numeric_limits<T>::is_specialized &&
                    std::numeric_limits<U>::is_specialized &&
                    std::numeric_limits<T>::digits <=
                        std::numeric_limits<U>::digits> {};

template <class T, class U>
constexpr bool is_upcastable_v = is_upcastable<T, U>::value;

using undefined = void;

template <class T>
struct DoublePrecision {
  using type = undefined;
};

template <>
struct DoublePrecision<std::uint8_t> {
  using type = std::uint16_t;
};

template <>
struct DoublePrecision<std::uint16_t> {
  using type = std::uint32_t;
};

template <>
struct DoublePrecision<std::uint32_t> {
  using type = std::uint64_t;
};

template <class T>
using DoublePrecisionOf = typename DoublePrecision<T>::type;

template <class T>
using ConstParameterFrom =
    std::conditional_t<std::is_arithmetic_v<T>, const T, const T&>;

static constexpr std::size_t BIT_LENGTHS_TABLE[32] = {
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

template <class T>
constexpr std::size_t bit_length(const T value) {
  std::size_t result = 0;
  T step = value;
  while (step >= 32) {
    result += 6;
    step >>= 6;
  }
  result += BIT_LENGTHS_TABLE[step];
  return result;
}

template <class Number>
constexpr Number const_power(const Number base, const std::size_t exponent) {
  Number result{1};
  if (!exponent) return result;
  std::size_t exponent_mask = 1 << (bit_length(exponent) - 1);
  while (exponent_mask) {
    result *= result;
    if (exponent & exponent_mask) result *= base;
    exponent_mask >>= 1;
  }
  return result;
}

template <class Number>
double divide_as_double(ConstParameterFrom<Number> dividend,
                        ConstParameterFrom<Number> divisor) {
  return dividend / divisor;
}

template <class Dividend, class Divisor = Dividend,
          std::enable_if_t<std::is_integral_v<Dividend> &&
                               std::is_integral_v<Divisor> &&
                               std::is_unsigned_v<Dividend>,
                           int> = 0>
Dividend floor_divide(ConstParameterFrom<Dividend> dividend,
                      ConstParameterFrom<Divisor> divisor) {
  return dividend / divisor;
}

template <
    class Dividend, class Divisor = Dividend,
    std::enable_if_t<
        std::is_same_v<std::invoke_result_t<decltype(&Dividend::floor_divide),
                                            ConstParameterFrom<Dividend>,
                                            ConstParameterFrom<Divisor>>,
                       Dividend>,
        int> = 0>
Dividend floor_divide(ConstParameterFrom<Dividend> dividend,
                      ConstParameterFrom<Divisor> divisor) {
  return dividend.floor_divide(divisor);
}

template <std::size_t BASE>
constexpr std::size_t floor_log(const std::size_t value) noexcept {
  return value < BASE ? 0 : floor_log<BASE>(value / BASE) + 1;
}

std::size_t floor_log2(std::size_t value) noexcept {
  std::size_t result = 0;
  for (; value; ++result, value >>= 1)
    ;
  return result - 1;
}

template <class T>
T gcd(T first, T second) noexcept {
  while (!!second) {
    const T step = second;
    second = first % second;
    first = step;
  }
  return first;
}

template <class Number>
bool is_negative(ConstParameterFrom<Number> value) noexcept {
  static const Number ZERO{};
  return value < ZERO;
}

template <class Number>
bool is_one(ConstParameterFrom<Number> value) noexcept {
  static const Number ONE{1};
  return value == ONE;
}

template <class Number>
bool is_positive(ConstParameterFrom<Number> value) noexcept {
  static const Number ZERO{};
  return value > ZERO;
}

template <class Dividend, class Divisor = Dividend,
          std::enable_if_t<std::is_integral_v<Dividend> &&
                               std::is_integral_v<Divisor> &&
                               std::is_signed_v<Dividend>,
                           int> = 0>
Dividend floor_divide(ConstParameterFrom<Dividend> dividend,
                      ConstParameterFrom<Divisor> divisor) {
  Dividend result = dividend / divisor, remainder = dividend % divisor;
  return result -
         ((is_negative<Divisor>(divisor) && is_positive<Dividend>(remainder)) ||
          (is_positive<Divisor>(divisor) && is_negative<Dividend>(remainder)));
}

template <class Dividend, class Divisor = Dividend,
          std::enable_if_t<std::is_integral_v<Dividend> &&
                               std::is_integral_v<Divisor> &&
                               std::is_unsigned_v<Dividend>,
                           int> = 0>
Dividend mod(ConstParameterFrom<Dividend> dividend,
             ConstParameterFrom<Divisor> divisor) {
  return dividend % divisor;
}

template <class Dividend, class Divisor = Dividend,
          std::enable_if_t<
              std::is_same_v<std::invoke_result_t<decltype(&Dividend::mod),
                                                  ConstParameterFrom<Dividend>,
                                                  ConstParameterFrom<Divisor>>,
                             Dividend>,
              int> = 0>
Dividend mod(ConstParameterFrom<Dividend> dividend,
             ConstParameterFrom<Divisor> divisor) {
  return dividend.mod(divisor);
}

template <class Dividend, class Divisor = Dividend,
          std::enable_if_t<std::is_integral_v<Dividend> &&
                               std::is_integral_v<Divisor> &&
                               std::is_signed_v<Dividend>,
                           int> = 0>
Dividend mod(ConstParameterFrom<Dividend> dividend,
             ConstParameterFrom<Divisor> divisor) {
  Dividend result = dividend % divisor;
  return result +
         ((is_negative<Divisor>(divisor) && is_positive<Dividend>(result)) ||
          (is_positive<Divisor>(divisor) && is_negative<Dividend>(result))) *
             divisor;
}

template <class Number>
Number power(ConstParameterFrom<Number> number,
             ConstParameterFrom<Number> exponent) {
  assert(!is_negative(exponent));
  return const_power(number, static_cast<std::size_t>(exponent));
}
}  // namespace cppbuiltins

#endif
