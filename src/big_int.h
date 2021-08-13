#ifndef INT_HPP
#define INT_HPP

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "utils.h"

constexpr bool ASCII_CODES_WHITESPACE_FLAGS[256] = {
    false, false, false, false, false, false, false, false, false, true,  true,
    true,  true,  true,  false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, true,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false,
};
constexpr std::size_t MAX_REPRESENTABLE_BASE = 36;
constexpr unsigned char ASCII_CODES_DIGIT_VALUES[256] = {
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 0,  1,  2,  3,  4,  5,  6,  7,  8,
    9,  37, 37, 37, 37, 37, 37, 37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37,
    37, 37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
    27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37,
};

static constexpr unsigned char mask_char(char character) {
  return static_cast<unsigned char>(character & 0xff);
}

static constexpr bool is_space(char character) {
  return ASCII_CODES_WHITESPACE_FLAGS[mask_char(character)];
}

template <class SourceDigit, class TargetDigit, std::size_t SOURCE_SHIFT,
          std::size_t TARGET_SHIFT,
          TargetDigit TARGET_DIGIT_MASK = power(TargetDigit(2), TARGET_SHIFT) -
                                          1>
static std::vector<SourceDigit> binary_digits_to_greater_binary_base(
    const std::vector<SourceDigit>& source) {
  static_assert(SOURCE_SHIFT < TARGET_SHIFT,
                "Target base should be greater than a source one.");
  const std::size_t result_digits_count = static_cast<std::size_t>(
      (source.size() * TARGET_SHIFT + TARGET_SHIFT - 1) / TARGET_SHIFT);
  std::vector<TargetDigit> result;
  result.reserve(result_digits_count);
  double_precision_t<TargetDigit> accumulator = 0;
  std::size_t accumulator_bits_count = 0;
  for (const SourceDigit digit : source) {
    accumulator |= static_cast<double_precision_t<TargetDigit>>(digit)
                   << accumulator_bits_count;
    accumulator_bits_count += SOURCE_SHIFT;
    if (accumulator_bits_count >= TARGET_SHIFT) {
      result.push_back(
          static_cast<TargetDigit>(accumulator & TARGET_DIGIT_MASK));
      accumulator >>= TARGET_SHIFT;
      accumulator_bits_count -= TARGET_SHIFT;
    }
  }
  if (accumulator) result.push_back(accumulator);
  return result;
}

template <class SourceDigit, class TargetDigit, std::size_t SOURCE_SHIFT,
          std::size_t TARGET_SHIFT,
          std::size_t TARGET_DIGIT_MASK = (TargetDigit(1) << TARGET_SHIFT) - 1>
static std::vector<TargetDigit> binary_digits_to_lesser_binary_base(
    const std::vector<SourceDigit>& source) {
  static_assert(SOURCE_SHIFT > TARGET_SHIFT,
                "Target base should be lesser than a source one.");
  const std::size_t result_digits_bits_count =
      ((source.size() - 1) * SOURCE_SHIFT + to_bit_length(source.back()));
  const std::size_t result_digits_count = static_cast<std::size_t>(
      (result_digits_bits_count + (TARGET_SHIFT - 1)) / TARGET_SHIFT);
  std::vector<TargetDigit> result;
  result.reserve(result_digits_count);
  double_precision_t<SourceDigit> accumulator = 0;
  std::size_t accumulator_bits_count = 0;
  for (std::size_t index = 0; index < source.size(); ++index) {
    accumulator |= static_cast<double_precision_t<SourceDigit>>(source[index])
                   << accumulator_bits_count;
    accumulator_bits_count += SOURCE_SHIFT;
    do {
      result.push_back(
          static_cast<TargetDigit>(accumulator & TARGET_DIGIT_MASK));
      accumulator_bits_count -= TARGET_SHIFT;
      accumulator >>= TARGET_SHIFT;
    } while (index == source.size() - 1
                 ? accumulator != 0
                 : accumulator_bits_count >= TARGET_SHIFT);
  }
  return result;
}

template <class SourceDigit, class TargetDigit, std::size_t SOURCE_SHIFT,
          std::size_t TARGET_SHIFT>
std::vector<TargetDigit> binary_digits_to_binary_base(
    const std::vector<SourceDigit>& source) {
  if constexpr (SOURCE_SHIFT < TARGET_SHIFT)
    return binary_digits_to_greater_binary_base<SourceDigit, TargetDigit,
                                                SOURCE_SHIFT, TARGET_SHIFT>(
        source);
  else if constexpr (SOURCE_SHIFT > TARGET_SHIFT)
    return binary_digits_to_lesser_binary_base<SourceDigit, TargetDigit,
                                               SOURCE_SHIFT, TARGET_SHIFT>(
        source);
  else
    return source;
}

template <class SourceDigit, class TargetDigit, std::size_t SOURCE_SHIFT,
          std::size_t TARGET_BASE>
std::vector<TargetDigit> binary_digits_to_non_binary_base(
    const std::vector<SourceDigit>& source) {
  std::size_t result_max_digits_count =
      1 + static_cast<std::size_t>(source.size() * SOURCE_SHIFT /
                                   std::log2(TARGET_BASE));
  std::vector<TargetDigit> result;
  result.reserve(result_max_digits_count);
  using Digit =
      std::conditional_t <
      std::numeric_limits<SourceDigit>::digits<
          std::numeric_limits<TargetDigit>::digits, TargetDigit, SourceDigit>;
  for (auto iterator = source.rbegin(); iterator != source.rend(); ++iterator) {
    Digit digit = *iterator;
    for (std::size_t index = 0; index < result.size(); ++index) {
      double_precision_t<TargetDigit> step =
          (static_cast<double_precision_t<TargetDigit>>(result[index])
           << SOURCE_SHIFT) |
          digit;
      digit = step / TARGET_BASE;
      result[index] = static_cast<TargetDigit>(
          step -
          static_cast<double_precision_t<TargetDigit>>(digit) * TARGET_BASE);
    }
    while (digit) {
      result.push_back(digit % TARGET_BASE);
      digit /= TARGET_BASE;
    }
  }
  if (result.empty()) result.push_back(0);
  return result;
}

template <typename Digit>
static bool digits_lesser_than(const std::vector<Digit>& first,
                               const std::vector<Digit>& second) {
  return first.size() < second.size() ||
         (first.size() == second.size() &&
          std::lexicographical_compare(first.rbegin(), first.rend(),
                                       second.rbegin(), second.rend()));
}

template <typename Digit>
static bool digits_lesser_than_or_equal(const std::vector<Digit>& first,
                                        const std::vector<Digit>& second) {
  return first.size() < second.size() ||
         (first.size() == second.size() &&
          !std::lexicographical_compare(second.rbegin(), second.rend(),
                                        first.rbegin(), first.rend()));
}

template <
    class _Digit, char _SEPARATOR,
    std::size_t _BINARY_SHIFT =
        std::numeric_limits<typename std::make_signed<_Digit>::type>::digits -
        1>
class BigInt {
 public:
  static_assert(std::is_integral<_Digit>() && std::is_unsigned<_Digit>(),
                "Digits should be unsigned integrals.");
  using Digit = _Digit;
  using SignedDigit = typename std::make_signed<Digit>::type;

  static_assert(ASCII_CODES_DIGIT_VALUES[mask_char(_SEPARATOR)] >
                    MAX_REPRESENTABLE_BASE,
                "Separator should not be a digit");
  static constexpr char SEPARATOR = _SEPARATOR;

  static_assert(_BINARY_SHIFT <= std::numeric_limits<SignedDigit>::digits - 1,
                "Digit should be able to hold all integers lesser than double "
                "signed base.");
  static constexpr Digit BINARY_SHIFT = _BINARY_SHIFT;

  using DoubleDigit = double_precision_t<Digit>;
  static_assert(!std::is_same<DoubleDigit, undefined>(),
                "Double precision version of digit type is undefined.");
  static_assert(std::is_integral<DoubleDigit>(),
                "Double precision digit should be integral.");
  static_assert(std::numeric_limits<DoubleDigit>::digits >= 2 * BINARY_SHIFT,
                "Double precision digit should be able to hold all integers "
                "lesser than squared base.");
  using SignedDoubleDigit = typename std::make_signed<DoubleDigit>::type;

  static constexpr Digit BINARY_BASE = 1 << BINARY_SHIFT;
  static constexpr Digit BINARY_DIGIT_MASK = BINARY_BASE - 1;

  BigInt() : _sign(0), _digits({0}) {}

  template <class T, std::enable_if_t<std::is_same<T, Digit>::value, T> = 0>
  explicit BigInt(T value) {
    if (value == 0) {
      _sign = 0;
      _digits = {0};
    } else {
      _sign = 1;
      Digit remainder = value >> BINARY_SHIFT;
      if (remainder) {
        _digits.push_back(value & BINARY_DIGIT_MASK);
        _digits.push_back(remainder);
      } else
        _digits.push_back(value);
    }
  }

  template <class T,
            std::enable_if_t<std::is_same<T, DoubleDigit>::value, T> = 0>
  explicit BigInt(T value) {
    if (value == 0) {
      _sign = 0;
      _digits = {0};
    } else {
      _sign = 1;
      while (value) {
        _digits.push_back(static_cast<Digit>(value & BINARY_DIGIT_MASK));
        value >>= BINARY_SHIFT;
      }
    }
  }

  template <class T,
            std::enable_if_t<std::is_same<T, SignedDigit>::value, T> = 0>
  explicit BigInt(T value) {
    Digit modulus;
    if (value < 0) {
      modulus = -static_cast<Digit>(value);
      _sign = -1;
    } else {
      modulus = static_cast<Digit>(value);
      _sign = value == 0 ? 0 : 1;
    }
    Digit remainder = modulus >> BINARY_SHIFT;
    if (remainder) {
      _digits.push_back(modulus & BINARY_DIGIT_MASK);
      _digits.push_back(remainder);
    } else
      _digits.push_back(modulus);
  }

  template <class T,
            std::enable_if_t<std::is_same<T, SignedDoubleDigit>::value, T> = 0>
  explicit BigInt(T value) {
    DoubleDigit modulus;
    if (value < 0) {
      _sign = -1;
      modulus = static_cast<DoubleDigit>(-1 - value) + 1;
    } else if (value > 0) {
      _sign = 1;
      modulus = static_cast<DoubleDigit>(value);
    } else {
      _sign = 0;
      _digits = {0};
      return;
    }
    DoubleDigit accumulator = modulus;
    while (accumulator) {
      _digits.push_back(static_cast<Digit>(accumulator & BINARY_DIGIT_MASK));
      accumulator >>= BINARY_SHIFT;
    }
  }

  BigInt(const BigInt& value) : _sign(value._sign), _digits(value._digits) {}

  BigInt(const char* characters, std::size_t base = 10) {
    if ((base != 0 && base < 2) || base > MAX_REPRESENTABLE_BASE)
      throw std::invalid_argument(
          "Base should be zero or in range from 2 to 36.");
    const char* start = characters;
    while (*start != '\0' && is_space(*start)) start++;
    _sign = 1;
    if (*start == '+')
      ++start;
    else if (*start == '-') {
      _sign = -1;
      ++start;
    }
    if (base == 0) {
      if (start[0] != '0')
        base = 10;
      else if (start[1] == 'x' || start[1] == 'X')
        base = 16;
      else if (start[1] == 'o' || start[1] == 'O')
        base = 8;
      else if (start[1] == 'b' || start[1] == 'B')
        base = 2;
      else
        base = 10;
    }
    if (start[0] == '0' &&
        ((base == 16 && (start[1] == 'x' || start[1] == 'X')) ||
         (base == 8 && (start[1] == 'o' || start[1] == 'O')) ||
         (base == 2 && (start[1] == 'b' || start[1] == 'B')))) {
      start += 2;
      start += (*start == SEPARATOR);
    }
    if (*start == SEPARATOR)
      throw std::invalid_argument("Should not start with separator.");
    const char* stop = start;
    char prev = *start;
    std::size_t digits_count = 0;
    while (*stop != '\0' &&
           (ASCII_CODES_DIGIT_VALUES[mask_char(*stop)] < base ||
            *stop == SEPARATOR)) {
      if (*stop != SEPARATOR)
        ++digits_count;
      else if (prev == SEPARATOR)
        throw std::invalid_argument("Consecutive separators found.");
      prev = *stop;
      ++stop;
    }
    if (!digits_count) throw std::invalid_argument("No digits found.");
    if (prev == SEPARATOR)
      throw std::invalid_argument("Should not end with separator.");
    const char* cursor = stop;
    while (*cursor != '\0' && is_space(*cursor)) ++cursor;
    if (*cursor != '\0')
      throw std::invalid_argument("Should not end with non-whitespaces.");
    if (base & (base - 1))
      parse_non_binary_base_digits(start, stop, base, digits_count);
    else
      parse_binary_base_digits(start, stop, base, digits_count);
    normalize_digits(_digits);
    _sign *= (_digits.size() > 1 || _digits[0] != 0);
  }

  BigInt gcd(const BigInt& other) const {
    std::vector<Digit> largest_digits = _digits,
                       smallest_digits = other._digits;
    if (digits_lesser_than(largest_digits, smallest_digits))
      std::swap(largest_digits, smallest_digits);
    std::size_t largest_digits_count, smallest_digits_count;
    while ((largest_digits_count = largest_digits.size()) > 2) {
      smallest_digits_count = smallest_digits.size();
      if (smallest_digits_count == 1 && smallest_digits[0] == 0)
        return BigInt(1, largest_digits);
      std::size_t highest_digit_bit_length =
          to_bit_length(largest_digits.back());
      SignedDoubleDigit largest_leading_bits =
          ((static_cast<DoubleDigit>(largest_digits[largest_digits_count - 1])
            << (2 * BINARY_SHIFT - highest_digit_bit_length)) |
           (static_cast<DoubleDigit>(largest_digits[largest_digits_count - 2])
            << (BINARY_SHIFT - highest_digit_bit_length)) |
           (largest_digits[largest_digits_count - 3] >>
            highest_digit_bit_length));
      SignedDoubleDigit smallest_leading_bits =
          ((smallest_digits_count >= largest_digits_count - 2
                ? smallest_digits[largest_digits_count - 3] >>
                      highest_digit_bit_length
                : 0) |
           (smallest_digits_count >= largest_digits_count - 1
                ? static_cast<DoubleDigit>(
                      smallest_digits[largest_digits_count - 2])
                      << (BINARY_SHIFT - highest_digit_bit_length)
                : 0) |
           (smallest_digits_count >= largest_digits_count
                ? static_cast<DoubleDigit>(
                      smallest_digits[largest_digits_count - 1])
                      << (2 * BINARY_SHIFT - highest_digit_bit_length)
                : 0));
      SignedDoubleDigit A = 1, B = 0, C = 0, D = 1;
      std::size_t iterations_count = 0;
      for (;; ++iterations_count) {
        if (smallest_leading_bits == C) break;
        SignedDoubleDigit q =
            (largest_leading_bits + (A - 1)) / (smallest_leading_bits - C);
        SignedDoubleDigit s = B + q * D;
        SignedDoubleDigit step =
            largest_leading_bits - q * smallest_leading_bits;
        if (s > step) break;
        largest_leading_bits = smallest_leading_bits;
        smallest_leading_bits = step;
        step = A + q * C;
        A = D;
        B = C;
        C = s;
        D = step;
      }
      if (iterations_count == 0) {
        if (smallest_digits_count == 1) {
          std::vector<Digit> quotient;
          Digit remainder = divmod_digits_by_digit(
              largest_digits, smallest_digits[0], quotient);
          largest_digits = smallest_digits;
          smallest_digits = std::vector<Digit>({remainder});
        } else {
          std::vector<Digit> quotient, remainder;
          divmod_two_or_more_digits(largest_digits, smallest_digits, quotient,
                                    remainder);
          largest_digits = smallest_digits;
          smallest_digits = remainder;
        }
        continue;
      }
      if (iterations_count & 1) {
        A = -A;
        B = -B;
        C = -C;
        D = -D;
        std::swap(A, B);
        std::swap(C, D);
      }
      SignedDoubleDigit next_largest_accumulator = 0;
      SignedDoubleDigit next_smallest_accumulator = 0;
      std::size_t index = 0;
      std::vector<Digit> next_largest_digits, next_smallest_digits;
      next_largest_digits.reserve(largest_digits_count);
      next_smallest_digits.reserve(largest_digits_count);
      for (; index < smallest_digits_count; ++index) {
        next_largest_accumulator +=
            (A * largest_digits[index]) - (B * smallest_digits[index]);
        next_smallest_accumulator +=
            (D * smallest_digits[index]) - (C * largest_digits[index]);
        next_largest_digits.push_back(
            static_cast<Digit>(next_largest_accumulator & BINARY_DIGIT_MASK));
        next_smallest_digits.push_back(
            static_cast<Digit>(next_smallest_accumulator & BINARY_DIGIT_MASK));
        next_largest_accumulator >>= BINARY_SHIFT;
        next_smallest_accumulator >>= BINARY_SHIFT;
      }
      for (; index < largest_digits_count; ++index) {
        next_largest_accumulator += A * largest_digits[index];
        next_smallest_accumulator -= C * largest_digits[index];
        next_largest_digits.push_back(
            static_cast<Digit>(next_largest_accumulator & BINARY_DIGIT_MASK));
        next_smallest_digits.push_back(
            static_cast<Digit>(next_smallest_accumulator & BINARY_DIGIT_MASK));
        next_largest_accumulator >>= BINARY_SHIFT;
        next_smallest_accumulator >>= BINARY_SHIFT;
      }
      normalize_digits(next_largest_digits);
      normalize_digits(next_smallest_digits);
      largest_digits = next_largest_digits;
      smallest_digits = next_smallest_digits;
    }
    return BigInt(to_gcd(reduce_digits<DoubleDigit>(largest_digits),
                         reduce_digits<DoubleDigit>(smallest_digits)));
  }

  void divmod(const BigInt& divisor, BigInt& quotient,
              BigInt& remainder) const {
    divmod<true, true>(divisor, &quotient, &remainder);
  }

  BigInt floor_divide(const BigInt& divisor) const {
    BigInt result;
    divmod<true, false>(divisor, &result, nullptr);
    return result;
  }

  BigInt invmod(const BigInt& divisor) const {
    BigInt candidate, result{1u}, step_dividend = *this, step_divisor = divisor;
    while (step_divisor) {
      BigInt quotient, remainder;
      step_dividend.divmod(step_divisor, quotient, remainder);
      step_dividend = step_divisor;
      step_divisor = remainder;
      const BigInt next_candidate = result - quotient * candidate;
      result = candidate;
      candidate = next_candidate;
    }
    if (step_dividend.is_one()) {
      if (result.sign() < 0) result = result % divisor;
      return result;
    }
    throw std::invalid_argument("Not invertible.");
  }

  BigInt operator+(const BigInt& other) const {
    if (_digits.size() == 1 && other._digits.size() == 1)
      return BigInt(signed_digit() + other.signed_digit());
    if (_sign < 0) {
      if (other._sign < 0)
        return BigInt(-1, sum_digits(_digits, other._digits));
      else {
        int sign = 1;
        std::vector<Digit> digits =
            subtract_digits(other._digits, _digits, sign);
        return BigInt(sign, digits);
      }
    } else if (other._sign < 0) {
      int sign = 1;
      std::vector<Digit> digits = subtract_digits(_digits, other._digits, sign);
      return BigInt(sign, digits);
    } else
      return BigInt(_sign | other._sign, sum_digits(_digits, other._digits));
  }

  operator bool() const { return bool(_sign); }

  operator double() const {
    if (_digits.size() == 1) return static_cast<double>(signed_digit());
    int exponent;
    double fraction = frexp(exponent);
    return std::ldexp(fraction, exponent);
  }

  BigInt operator*(const BigInt& other) const {
    return _digits.size() == 1 && other._digits.size() == 1
               ? BigInt(signed_double_digit() * other.signed_double_digit())
               : BigInt(_sign * other._sign,
                        multiply_digits(_digits, other._digits));
  }

  BigInt operator-() const { return BigInt(-_sign, _digits); }

  BigInt operator~() const {
    if (_digits.size() == 1) return BigInt(-signed_digit() - 1);
    if (_sign > 0)
      return BigInt(-1, sum_digits(_digits, {1}));
    else {
      int sign = 1;
      std::vector<Digit> digits = subtract_digits(_digits, {1}, sign);
      return BigInt(sign, digits);
    }
  }

  BigInt operator-(const BigInt& other) const {
    if (_digits.size() == 1 && other._digits.size() == 1)
      return BigInt(signed_digit() - other.signed_digit());
    if (_sign < 0) {
      if (other._sign < 0) {
        int sign = 1;
        std::vector<Digit> digits =
            subtract_digits(other._digits, _digits, sign);
        return BigInt(sign, digits);
      } else
        return BigInt(-1, sum_digits(_digits, other._digits));
    } else if (other._sign < 0)
      return BigInt(1, sum_digits(_digits, other._digits));
    else {
      int sign = _sign | other._sign;
      std::vector<Digit> digits = subtract_digits(_digits, other._digits, sign);
      return BigInt(sign, digits);
    }
  }

  bool operator==(const BigInt& other) const {
    return _sign == other._sign && _digits == other._digits;
  }

  bool operator<(const BigInt& other) const {
    return _sign < other._sign ||
           (_sign == other._sign &&
            (_sign > 0 ? digits_lesser_than(_digits, other._digits)
                       : digits_lesser_than(other._digits, _digits)));
  }

  bool operator<=(const BigInt& other) const {
    return _sign < other._sign ||
           (_sign == other._sign &&
            (_sign > 0 ? digits_lesser_than_or_equal(_digits, other._digits)
                       : digits_lesser_than_or_equal(other._digits, _digits)));
  }

  BigInt operator%(const BigInt& divisor) const {
    BigInt result;
    divmod<false, true>(divisor, nullptr, &result);
    return result;
  }

  double operator/(const BigInt& divisor) const {
    if (divisor._sign == 0)
      throw std::range_error("Division by zero is undefined.");
    bool negate = (_sign < 0) ^ (divisor._sign < 0);
    if (sign() == 0) return negate ? -0.0 : 0.0;
    const std::vector<Digit>& dividend_digits = digits();
    const std::vector<Digit>& divisor_digits = divisor.digits();
    std::size_t dividend_digits_count = dividend_digits.size();
    std::size_t divisor_digits_count = divisor_digits.size();
    bool dividend_is_small =
        dividend_digits_count <= MANTISSA_BINARY_DIGITS_COUNT ||
        (dividend_digits_count == MANTISSA_BINARY_DIGITS_COUNT + 1 &&
         (dividend_digits[MANTISSA_BINARY_DIGITS_COUNT] >>
          MANTISSA_EXTRA_BITS) == 0);
    bool divisor_is_small =
        divisor_digits_count <= MANTISSA_BINARY_DIGITS_COUNT ||
        (divisor_digits_count == MANTISSA_BINARY_DIGITS_COUNT + 1 &&
         (divisor_digits[MANTISSA_BINARY_DIGITS_COUNT] >>
          MANTISSA_EXTRA_BITS) == 0);
    if (dividend_is_small && divisor_is_small) {
      double reduced_dividend = reduce_digits<double>(dividend_digits);
      double reduced_divisor = reduce_digits<double>(divisor_digits);
      double result = reduced_dividend / reduced_divisor;
      return negate ? -result : result;
    }
    std::make_signed_t<std::size_t> digits_count_difference =
        dividend_digits_count - divisor_digits_count;
    if (digits_count_difference >
        static_cast<std::make_signed_t<std::size_t>>(
            std::numeric_limits<std::size_t>::max() / BINARY_SHIFT) -
            1)
      throw std::overflow_error(
          "Division result too large to be expressed as floating point.");
    else if (digits_count_difference <
             1 - static_cast<std::make_signed_t<std::size_t>>(
                     std::numeric_limits<std::size_t>::max() / BINARY_SHIFT))
      return negate ? -0.0 : 0.0;
    std::make_signed_t<std::size_t> bit_lengths_difference =
        digits_count_difference * BINARY_SHIFT +
        (to_bit_length(dividend_digits.back()) -
         to_bit_length(divisor_digits.back()));
    if (bit_lengths_difference > std::numeric_limits<double>::max_exponent)
      throw std::overflow_error(
          "Division result too large to be expressed as floating point.");
    else if (bit_lengths_difference <
             static_cast<std::make_signed_t<std::size_t>>(
                 std::numeric_limits<double>::min_exponent - MANTISSA_BITS - 1))
      return negate ? -0.0 : 0.0;
    std::make_signed_t<std::size_t> shift =
        std::max(bit_lengths_difference,
                 static_cast<std::make_signed_t<std::size_t>>(
                     std::numeric_limits<double>::min_exponent)) -
        MANTISSA_BITS - 2;
    bool inexact = false;
    Digit* quotient_data;
    std::size_t quotient_digits_count;
    if (shift <= 0) {
      std::size_t shift_digits = (-shift) / BINARY_SHIFT;
      if (dividend_digits_count >=
          std::numeric_limits<std::make_signed_t<std::size_t>>::max() - 1 -
              shift_digits)
        throw std::overflow_error(
            "Division result too large to be expressed as floating point.");
      quotient_digits_count = dividend_digits_count + shift_digits + 1;
      quotient_data = new Digit[quotient_digits_count]();
      Digit remainder = shift_digits_left(
          dividend_digits.data(), dividend_digits_count,
          (-shift) % BINARY_SHIFT, quotient_data + shift_digits);
      quotient_data[dividend_digits_count + shift_digits] = remainder;
    } else {
      std::size_t shift_digits = shift / BINARY_SHIFT;
      quotient_digits_count = dividend_digits_count - shift_digits;
      quotient_data = new Digit[quotient_digits_count]();
      Digit remainder = shift_digits_right(
          dividend_digits.data() + shift_digits, quotient_digits_count,
          shift % BINARY_SHIFT, quotient_data);
      if (remainder) inexact = true;
      while (!inexact && shift_digits > 0)
        if (dividend_digits[--shift_digits]) inexact = true;
    }
    std::vector<Digit> quotient_digits(quotient_data,
                                       quotient_data + quotient_digits_count);
    normalize_digits(quotient_digits);
    if (divisor_digits_count == 1) {
      std::vector<Digit> next_quotient_digits;
      Digit remainder = divmod_digits_by_digit(
          quotient_digits, divisor_digits[0], next_quotient_digits);
      std::swap(quotient_digits, next_quotient_digits);
      if (remainder) inexact = true;
    } else {
      std::vector<Digit> next_quotient_digits, remainder;
      divmod_two_or_more_digits(quotient_digits, divisor_digits,
                                next_quotient_digits, remainder);
      std::swap(quotient_digits, next_quotient_digits);
      if (remainder.size() > 1 || remainder[0] != 0) inexact = true;
    }
    std::make_signed_t<std::size_t> quotient_bit_length =
        (quotient_digits.size() - 1) * BINARY_SHIFT +
        to_bit_length(quotient_digits.back());
    std::make_signed_t<std::size_t> extra_bits =
        std::max(quotient_bit_length,
                 std::numeric_limits<double>::min_exponent - shift) -
        MANTISSA_BITS;
    const Digit mask = static_cast<Digit>(1) << (extra_bits - 1);
    Digit quotient_low_digit = quotient_digits[0] | inexact;
    if ((quotient_low_digit & mask) && (quotient_low_digit & (3U * mask - 1U)))
      quotient_low_digit += mask;
    quotient_digits[0] = quotient_low_digit & ~(2U * mask - 1U);
    double reduced_quotient = reduce_digits<double>(quotient_digits);
    if (shift + quotient_bit_length >=
            std::numeric_limits<double>::max_exponent &&
        (shift + quotient_bit_length >
             std::numeric_limits<double>::max_exponent ||
         reduced_quotient == ldexp(1.0, static_cast<int>(quotient_bit_length))))
      throw std::overflow_error(
          "Division result too large to be expressed as floating point.");
    double result = ldexp(reduced_quotient, static_cast<int>(shift));
    return negate ? -result : result;
  }

  BigInt abs() const { return _sign < 0 ? BigInt(1, _digits) : *this; }

  BigInt pow(BigInt exponent, const BigInt* maybe_modulus = nullptr) const {
    BigInt base = *this, modulus;
    bool is_negative = false;
    std::function<BigInt(const BigInt&, const BigInt&)> make_step;
    if (maybe_modulus != nullptr) {
      if (maybe_modulus->sign() == 0)
        throw std::invalid_argument("Modulus cannot be zero.");
      is_negative = maybe_modulus->sign() < 0;
      modulus = maybe_modulus->abs();
      if (modulus.is_one()) return BigInt();
      if (exponent.sign() < 0) {
        exponent = -exponent;
        base = base.invmod(modulus);
      }
      if (base.sign() < 0 || base.digits().size() > modulus.digits().size())
        base = base % modulus;
      make_step = [&modulus](const BigInt& first, const BigInt& second) {
        return (first * second) % modulus;
      };
    } else if (exponent.sign() < 0)
      throw std::range_error(
          "Exponent should be positive or modulus should be specified.");
    else
      make_step = [](const BigInt& first, const BigInt& second) {
        return first * second;
      };
    const std::vector<Digit>& exponent_digits = exponent.digits();
    Digit exponent_digit = exponent_digits.back();
    std::size_t exponent_digits_count = exponent_digits.size();
    BigInt result = BigInt(1u);
    if (exponent_digits_count == 1 && exponent_digit <= 3) {
      if (exponent_digit >= 2) {
        result = make_step(base, base);
        if (exponent_digit == 3) result = make_step(result, base);
      } else if (exponent_digit == 1)
        result = make_step(base, result);
    } else if (exponent_digits_count <= WINDOW_CUTOFF) {
      result = base;
      Digit bit = 2;
      for (;; bit <<= 1)
        if (bit > exponent_digit) {
          bit >>= 1;
          break;
        }
      bit >>= 1;
      for (auto exponent_digit_position = exponent_digits.rbegin();;) {
        for (; bit != 0; bit >>= 1) {
          result = make_step(result, result);
          if (exponent_digit & bit) result = make_step(result, base);
        }
        if (++exponent_digit_position == exponent_digits.rend()) break;
        exponent_digit = *exponent_digit_position;
        bit = static_cast<Digit>(1) << (BINARY_SHIFT - 1);
      }
    } else {
      BigInt cache[WINDOW_BASE];
      cache[0] = result;
      for (std::size_t index = 1; index < WINDOW_BASE; ++index)
        cache[index] = make_step(cache[index - 1], base);
      std::vector<WindowDigit> exponent_window_digits =
          binary_digits_to_binary_base<Digit, WindowDigit, BINARY_SHIFT,
                                       WINDOW_SHIFT>(exponent_digits);
      for (auto exponent_digit_position = exponent_window_digits.rbegin();
           exponent_digit_position != exponent_window_digits.rend();
           ++exponent_digit_position) {
        const WindowDigit digit = *exponent_digit_position;
        for (std::size_t iteration = 0; iteration < WINDOW_SHIFT; ++iteration)
          result = make_step(result, result);
        if (digit) result = make_step(result, cache[digit]);
      }
    }
    if (is_negative && result) result = result - modulus;
    return result;
  }

  template <std::size_t BASE = 10,
            std::size_t TARGET_SHIFT = floor_log<BASE>(BINARY_BASE),
            std::size_t TARGET_BASE = power(BASE, TARGET_SHIFT)>
  std::string repr() const {
    static_assert(1 < BASE && BASE < MAX_REPRESENTABLE_BASE,
                  "Base should be range from 2 to 36.");
    const std::vector<Digit> base_digits = to_base_digits<TARGET_BASE>();
    const std::size_t characters_count =
        (_sign < 0) + (base_digits.size() - 1) * TARGET_SHIFT +
        floor_log<BASE>(base_digits.back()) + 1;
    char* characters = new char[characters_count + 1]();
    char* stop = &characters[characters_count];
    for (std::size_t index = 0; index < base_digits.size() - 1; index++) {
      Digit remainder = base_digits[index];
      for (std::size_t step = 0; step < TARGET_SHIFT; step++) {
        *--stop = '0' + (remainder % BASE);
        remainder /= BASE;
      }
    }
    for (Digit remainder = base_digits.back(); remainder != 0;
         remainder /= BASE)
      *--stop = '0' + (remainder % BASE);
    if (_sign == 0)
      *--stop = '0';
    else if (_sign < 0)
      *--stop = '-';
    std::string result(characters, characters_count);
    delete[] characters;
    return result;
  }

 protected:
  BigInt(int sign, const std::vector<Digit>& digits)
      : _sign(sign), _digits(digits) {}

  const std::vector<Digit>& digits() const { return _digits; }

  bool is_one() const {
    return _sign > 0 && _digits.size() == 1 && _digits[0] == 1;
  }

  int sign() const { return _sign; }

  SignedDigit signed_digit() const {
    return _sign * static_cast<SignedDigit>(_digits[0]);
  }

  SignedDoubleDigit signed_double_digit() const {
    return _sign * static_cast<SignedDoubleDigit>(_digits[0]);
  }

 private:
  int _sign;
  std::vector<Digit> _digits;

  static constexpr Digit KARATSUBA_CUTOFF = 70;
  static constexpr Digit KARATSUBA_SQUARE_CUTOFF = KARATSUBA_CUTOFF * 2;
  static constexpr std::size_t MANTISSA_BITS =
      std::numeric_limits<double>::digits;
  static constexpr double MANTISSA_BITS_POWER_OF_TWO =
      power(2.0, MANTISSA_BITS);
  static constexpr std::size_t MANTISSA_BINARY_DIGITS_COUNT =
      MANTISSA_BITS / BINARY_SHIFT;
  static constexpr std::make_signed_t<std::size_t> MANTISSA_EXTRA_BITS =
      MANTISSA_BITS % BINARY_SHIFT;
  static constexpr std::size_t WINDOW_CUTOFF = 8;
  static constexpr std::size_t WINDOW_SHIFT = 5;
  static constexpr std::size_t WINDOW_BASE = 1 << WINDOW_SHIFT;
  static constexpr std::size_t WINDOW_DIGIT_MASK = WINDOW_BASE - 1;
  using WindowDigit = std::uint8_t;
  static_assert(WINDOW_SHIFT <= std::numeric_limits<WindowDigit>::digits,
                "Window digit type should be able to contain window digits.");

  static void divmod_two_or_more_digits(const std::vector<Digit>& dividend,
                                        const std::vector<Digit>& divisor,
                                        std::vector<Digit>& quotient,
                                        std::vector<Digit>& remainder) {
    std::size_t dividend_digits_count = dividend.size();
    std::size_t divisor_digits_count = divisor.size();
    Digit* dividend_normalized = new Digit[dividend_digits_count + 1]();
    Digit* divisor_normalized = new Digit[divisor_digits_count]();
    std::size_t shift = BINARY_SHIFT - to_bit_length(divisor.back());
    shift_digits_left(divisor.data(), divisor_digits_count, shift,
                      divisor_normalized);
    Digit accumulator = shift_digits_left(
        dividend.data(), dividend_digits_count, shift, dividend_normalized);
    if (accumulator != 0 || dividend_normalized[dividend_digits_count - 1] >=
                                divisor_normalized[divisor_digits_count - 1])
      dividend_normalized[dividend_digits_count++] = accumulator;
    std::size_t quotient_size = dividend_digits_count - divisor_digits_count;
    Digit* quotient_data = new Digit[quotient_size]();
    Digit last_divisor_digit_normalized =
        divisor_normalized[divisor_digits_count - 1];
    Digit penult_divisor_digit_normalized =
        divisor_normalized[divisor_digits_count - 2];
    for (Digit *digits_normalized_tail = dividend_normalized + quotient_size,
               *quotient_position = quotient_data + quotient_size;
         digits_normalized_tail-- > dividend_normalized;) {
      DoubleDigit step = (static_cast<DoubleDigit>(
                              digits_normalized_tail[divisor_digits_count])
                          << BINARY_SHIFT) |
                         digits_normalized_tail[divisor_digits_count - 1];
      Digit quotient_digit =
          static_cast<Digit>(step / last_divisor_digit_normalized);
      Digit step_remainder = static_cast<Digit>(
          step - static_cast<DoubleDigit>(last_divisor_digit_normalized) *
                     quotient_digit);
      while (static_cast<DoubleDigit>(penult_divisor_digit_normalized) *
                 quotient_digit >
             ((static_cast<DoubleDigit>(step_remainder) << BINARY_SHIFT) |
              digits_normalized_tail[divisor_digits_count - 2])) {
        --quotient_digit;
        step_remainder += last_divisor_digit_normalized;
        if (step_remainder >= BINARY_BASE) break;
      }
      SignedDigit accumulator = 0;
      for (std::size_t index = 0; index < divisor_digits_count; ++index) {
        SignedDoubleDigit step;
        step = static_cast<SignedDigit>(digits_normalized_tail[index]) +
               accumulator -
               static_cast<SignedDoubleDigit>(quotient_digit) *
                   static_cast<SignedDoubleDigit>(divisor_normalized[index]);
        digits_normalized_tail[index] =
            static_cast<Digit>(step) & BINARY_DIGIT_MASK;
        accumulator = static_cast<SignedDigit>(step >> BINARY_SHIFT);
      }
      if (static_cast<SignedDigit>(
              digits_normalized_tail[divisor_digits_count]) +
              accumulator <
          0) {
        Digit accumulator = 0;
        for (std::size_t index = 0; index < divisor_digits_count; ++index) {
          accumulator +=
              digits_normalized_tail[index] + divisor_normalized[index];
          digits_normalized_tail[index] = accumulator & BINARY_DIGIT_MASK;
          accumulator >>= BINARY_SHIFT;
        }
        --quotient_digit;
      }
      *--quotient_position = quotient_digit;
    }
    quotient = quotient_size ? std::vector<Digit>(quotient_data,
                                                  quotient_data + quotient_size)
                             : std::vector<Digit>({0});
    delete[] quotient_data;
    normalize_digits(quotient);
    shift_digits_right(dividend_normalized, divisor_digits_count, shift,
                       divisor_normalized);
    delete[] dividend_normalized;
    remainder = std::vector<Digit>(divisor_normalized,
                                   divisor_normalized + divisor_digits_count);
    delete[] divisor_normalized;
    normalize_digits(remainder);
  }

  static Digit divmod_digits_by_digit(const std::vector<Digit>& dividend,
                                      Digit divisor,
                                      std::vector<Digit>& quotient) {
    DoubleDigit remainder = 0;
    std::size_t digits_count = dividend.size();
    Digit* const quotient_data = new Digit[digits_count]();
    for (std::size_t offset = 1; offset <= digits_count; ++offset) {
      remainder = (remainder << BINARY_SHIFT) | dividend[digits_count - offset];
      Digit quotient_digit = quotient_data[digits_count - offset] =
          static_cast<Digit>(remainder / divisor);
      remainder -= static_cast<DoubleDigit>(quotient_digit) * divisor;
    }
    quotient =
        std::vector<Digit>(quotient_data, quotient_data + dividend.size());
    delete[] quotient_data;
    normalize_digits(quotient);
    return static_cast<Digit>(remainder);
  }

  template <class Result>
  static Result reduce_digits(const std::vector<Digit>& digits) {
    Result result = 0;
    for (auto position = digits.rbegin(); position != digits.rend(); ++position)
      if constexpr (std::is_integral<Result>())
        result = (result << BINARY_SHIFT) | *position;
      else
        result = (result * BINARY_BASE) + *position;
    return result;
  }

  static Digit subtract_digits_in_place(Digit* longest,
                                        std::size_t size_longest,
                                        const std::vector<Digit>& shortest) {
    Digit accumulator = 0;
    std::size_t index = 0;
    for (; index < shortest.size(); ++index) {
      accumulator = longest[index] - shortest[index] - accumulator;
      longest[index] = accumulator & BINARY_DIGIT_MASK;
      accumulator >>= BINARY_SHIFT;
      accumulator &= 1;
    }
    for (; accumulator && index < size_longest; ++index) {
      accumulator = longest[index] - accumulator;
      longest[index] = accumulator & BINARY_DIGIT_MASK;
      accumulator >>= BINARY_SHIFT;
      accumulator &= 1;
    }
    return accumulator;
  }

  static std::vector<Digit> subtract_digits(const std::vector<Digit>& first,
                                            const std::vector<Digit>& second,
                                            int& sign) {
    const std::vector<Digit>*longest = &first, *shortest = &second;
    std::size_t size_longest = longest->size(),
                size_shortest = shortest->size();
    Digit accumulator = 0;
    if (size_longest < size_shortest) {
      std::swap(longest, shortest);
      std::swap(size_longest, size_shortest);
      sign = -sign;
    } else if (size_longest == size_shortest) {
      std::size_t index = size_shortest;
      while (--index > 0 && (*longest)[index] == (*shortest)[index])
        ;
      if (index == 0 && (*longest)[0] == (*shortest)[0]) {
        sign = 0;
        return {0};
      }
      if ((*longest)[index] < (*shortest)[index]) {
        std::swap(longest, shortest);
        sign = -sign;
      }
      size_longest = size_shortest = index + 1;
    }
    std::vector<Digit> result;
    result.reserve(size_longest);
    std::size_t index = 0;
    for (; index < size_shortest; ++index) {
      accumulator = (*longest)[index] - (*shortest)[index] - accumulator;
      result.push_back(accumulator & BINARY_DIGIT_MASK);
      accumulator >>= BINARY_SHIFT;
      accumulator &= 1;
    }
    for (; index < size_longest; ++index) {
      accumulator = (*longest)[index] - accumulator;
      result.push_back(accumulator & BINARY_DIGIT_MASK);
      accumulator >>= BINARY_SHIFT;
      accumulator &= 1;
    }
    normalize_digits(result);
    return result;
  }

  static Digit sum_digits_in_place(Digit* longest, std::size_t size_longest,
                                   const std::vector<Digit>& shortest) {
    Digit accumulator = 0;
    std::size_t index = 0;
    for (; index < shortest.size(); ++index) {
      accumulator += longest[index] + shortest[index];
      longest[index] = accumulator & BINARY_DIGIT_MASK;
      accumulator >>= BINARY_SHIFT;
    }
    for (; accumulator && index < size_longest; ++index) {
      accumulator += longest[index];
      longest[index] = accumulator & BINARY_DIGIT_MASK;
      accumulator >>= BINARY_SHIFT;
    }
    return accumulator;
  }

  static std::vector<Digit> sum_digits(const std::vector<Digit>& first,
                                       const std::vector<Digit>& second) {
    const std::vector<Digit>*longest = &first, *shortest = &second;
    std::size_t size_longest = longest->size(),
                size_shortest = shortest->size();
    if (size_longest < size_shortest) {
      std::swap(size_longest, size_shortest);
      std::swap(longest, shortest);
    }
    std::vector<Digit> result;
    result.reserve(size_longest + 1);
    Digit accumulator = 0;
    std::size_t index = 0;
    for (; index < size_shortest; ++index) {
      accumulator += (*longest)[index] + (*shortest)[index];
      result.push_back(accumulator & BINARY_DIGIT_MASK);
      accumulator >>= BINARY_SHIFT;
    }
    for (; index < size_longest; ++index) {
      accumulator += (*longest)[index];
      result.push_back(accumulator & BINARY_DIGIT_MASK);
      accumulator >>= BINARY_SHIFT;
    }
    result.push_back(accumulator);
    normalize_digits(result);
    return result;
  }

  static void split_digits(const std::vector<Digit>& digits, std::size_t size,
                           std::vector<Digit>& high, std::vector<Digit>& low) {
    const std::size_t size_low = std::min<std::size_t>(digits.size(), size);
    const typename std::vector<Digit>::const_iterator mid =
        digits.begin() + size_low;
    low = std::vector<Digit>(digits.begin(), mid);
    high = std::vector<Digit>(mid, digits.end());
    normalize_digits(high);
    normalize_digits(low);
  }

  static std::vector<Digit> multiply_digits(const std::vector<Digit>& first,
                                            const std::vector<Digit>& second) {
    const std::vector<Digit>*shortest = &first, *longest = &second;
    std::size_t size_shortest = shortest->size(),
                size_longest = longest->size();
    if (size_longest < size_shortest) {
      std::swap(shortest, longest);
      std::swap(size_shortest, size_longest);
    }
    if (size_shortest <=
        ((shortest == longest) ? KARATSUBA_SQUARE_CUTOFF : KARATSUBA_CUTOFF)) {
      return size_shortest == 0 ? std::vector<Digit>({0})
                                : multiply_digits_plain(*shortest, *longest);
    }
    if (2 * size_shortest <= size_longest)
      return multiply_digits_lopsided(*shortest, *longest);
    const std::size_t shift = size_longest >> 1;
    std::vector<Digit> shortest_high, shortest_low;
    split_digits(*shortest, shift, shortest_high, shortest_low);
    std::vector<Digit> longest_high, longest_low;
    if (shortest == longest) {
      longest_high = shortest_high;
      longest_low = shortest_low;
    } else
      split_digits(*longest, shift, longest_high, longest_low);
    std::vector<Digit> result(size_shortest + size_longest, 0);
    const std::vector<Digit> highs_product =
        multiply_digits(shortest_high, longest_high);
    std::copy(highs_product.begin(), highs_product.end(),
              result.begin() + 2 * shift);
    const std::vector<Digit> lows_product =
        multiply_digits(shortest_low, longest_low);
    std::copy(lows_product.begin(), lows_product.end(), result.begin());
    const std::size_t digits_after_shift = result.size() - shift;
    (void)subtract_digits_in_place(result.data() + shift, digits_after_shift,
                                   lows_product);
    (void)subtract_digits_in_place(result.data() + shift, digits_after_shift,
                                   highs_product);
    const std::vector<Digit> shortest_components_sum =
        sum_digits(shortest_high, shortest_low);
    const std::vector<Digit> longest_components_sum =
        (shortest == longest) ? shortest_components_sum
                              : sum_digits(longest_high, longest_low);
    const std::vector<Digit> components_sums_product =
        multiply_digits(shortest_components_sum, longest_components_sum);
    (void)sum_digits_in_place(result.data() + shift, digits_after_shift,
                              components_sums_product);
    normalize_digits(result);
    return result;
  }

  static std::vector<Digit> multiply_digits_lopsided(
      const std::vector<Digit>& shortest, const std::vector<Digit>& longest) {
    const std::size_t size_shortest = shortest.size();
    std::size_t size_longest = longest.size();
    std::vector<Digit> result(size_shortest + size_longest, 0);
    std::size_t processed_digits_count = 0;
    while (size_longest > 0) {
      const std::size_t step_digits_count =
          std::min<std::size_t>(size_longest, size_shortest);
      const std::vector<Digit> step_digits(
          longest.begin() + processed_digits_count,
          longest.begin() + processed_digits_count + step_digits_count);
      const std::vector<Digit> product = multiply_digits(shortest, step_digits);
      (void)sum_digits_in_place(result.data() + processed_digits_count,
                                result.size() - processed_digits_count,
                                product);
      size_longest -= step_digits_count;
      processed_digits_count += step_digits_count;
    }
    normalize_digits(result);
    return result;
  }

  static std::vector<Digit> multiply_digits_plain(
      const std::vector<Digit>& first, const std::vector<Digit>& second) {
    std::size_t first_size = first.size(), second_size = second.size();
    std::vector<Digit> result(first_size + second_size, 0);
    if (&first == &second)
      for (std::size_t index = 0; index < first_size; ++index) {
        DoubleDigit accumulator;
        DoubleDigit digit = first[index];
        auto result_position = result.begin() + (index << 1);
        auto first_position = first.begin() + (index + 1);
        accumulator = *result_position + digit * digit;
        *(result_position++) =
            static_cast<Digit>(accumulator & BINARY_DIGIT_MASK);
        accumulator >>= BINARY_SHIFT;
        digit <<= 1;
        while (first_position != first.end()) {
          accumulator += *result_position + *(first_position++) * digit;
          *(result_position++) =
              static_cast<Digit>(accumulator & BINARY_DIGIT_MASK);
          accumulator >>= BINARY_SHIFT;
        }
        if (accumulator) {
          accumulator += *result_position;
          *(result_position++) =
              static_cast<Digit>(accumulator & BINARY_DIGIT_MASK);
          accumulator >>= BINARY_SHIFT;
        }
        if (accumulator)
          *result_position +=
              static_cast<Digit>(accumulator & BINARY_DIGIT_MASK);
      }
    else
      for (std::size_t index = 0; index < first_size; ++index) {
        DoubleDigit accumulator = 0;
        const DoubleDigit digit = first[index];
        auto result_position = result.begin() + index;
        auto second_position = second.begin();
        while (second_position != second.end()) {
          accumulator += *result_position + *(second_position++) * digit;
          *(result_position++) =
              static_cast<Digit>(accumulator & BINARY_DIGIT_MASK);
          accumulator >>= BINARY_SHIFT;
        }
        if (accumulator)
          *result_position +=
              static_cast<Digit>(accumulator & BINARY_DIGIT_MASK);
      }
    normalize_digits(result);
    return result;
  }

  static void normalize_digits(std::vector<Digit>& digits) {
    std::size_t digits_count = digits.size();
    while (digits_count > 1 && digits[digits_count - 1] == 0) --digits_count;
    if (digits_count != digits.size()) digits.resize(digits_count);
  }

  template <bool WITH_QUOTIENT, bool WITH_REMAINDER>
  void divmod(const BigInt& divisor, BigInt* quotient,
              BigInt* remainder) const {
    static_assert(WITH_QUOTIENT || WITH_REMAINDER,
                  "Quotient or remainder or both should be requested.");
    std::size_t digits_count = _digits.size(),
                divisor_digits_count = divisor._digits.size();
    if (divisor._sign == 0)
      throw std::range_error("Division by zero is undefined.");
    else if (_sign == 0) {
      if constexpr (WITH_QUOTIENT) *quotient = BigInt();
      if constexpr (WITH_REMAINDER) *remainder = *this;
    } else if (digits_count < divisor_digits_count ||
               (digits_count == divisor_digits_count &&
                _digits.back() < divisor._digits.back())) {
      if (_sign != divisor._sign) {
        if constexpr (WITH_QUOTIENT)
          *quotient = BigInt(-1, std::vector<Digit>({1}));
        if constexpr (WITH_REMAINDER) *remainder = *this + divisor;
      } else {
        if constexpr (WITH_QUOTIENT) *quotient = BigInt();
        if constexpr (WITH_REMAINDER) *remainder = *this;
      }
    } else {
      int remainder_sign = _sign;
      if (divisor_digits_count == 1) {
        std::vector<Digit> quotient_digits;
        Digit remainder_digit = divmod_digits_by_digit(
            _digits, divisor._digits[0], quotient_digits);
        remainder_sign *= remainder_digit != 0;
        if constexpr (WITH_QUOTIENT)
          *quotient = BigInt(_sign * divisor._sign, quotient_digits);
        if constexpr (WITH_REMAINDER)
          *remainder =
              BigInt(_sign * static_cast<SignedDigit>(remainder_digit));
      } else {
        std::vector<Digit> quotient_digits, remainder_digits;
        divmod_two_or_more_digits(_digits, divisor._digits, quotient_digits,
                                  remainder_digits);
        remainder_sign *=
            remainder_digits.size() > 1 || remainder_digits[0] != 0;
        if constexpr (WITH_QUOTIENT)
          *quotient = BigInt(
              _sign * divisor._sign *
                  (quotient_digits.size() > 1 || quotient_digits[0] != 0),
              quotient_digits);
        if constexpr (WITH_REMAINDER)
          *remainder = BigInt(remainder_sign, remainder_digits);
      }
      if ((divisor._sign < 0 && remainder_sign > 0) ||
          (divisor._sign > 0 && remainder_sign < 0)) {
        if constexpr (WITH_QUOTIENT)
          *quotient = *quotient - BigInt(1, std::vector<Digit>({1}));
        if constexpr (WITH_REMAINDER) *remainder = *remainder + divisor;
      }
    }
  }

  void parse_binary_base_digits(const char* start, const char* stop,
                                std::size_t base, std::size_t digits_count) {
    const std::size_t bits_per_character = floor_log2(base);
    if (digits_count >
        (std::numeric_limits<std::size_t>::max() - (BINARY_SHIFT - 1)) /
            bits_per_character)
      throw std::invalid_argument("Too many digits.");
    const std::size_t result_digits_count =
        (digits_count * bits_per_character + (BINARY_SHIFT - 1)) / BINARY_SHIFT;
    _digits.reserve(result_digits_count);
    DoubleDigit accumulator = 0;
    std::size_t bits_in_accumulator = 0;
    while (--stop >= start) {
      if (*stop == SEPARATOR) continue;
      accumulator |=
          static_cast<DoubleDigit>(ASCII_CODES_DIGIT_VALUES[mask_char(*stop)])
          << bits_in_accumulator;
      bits_in_accumulator += bits_per_character;
      if (bits_in_accumulator >= BINARY_SHIFT) {
        _digits.push_back(static_cast<Digit>(accumulator & BINARY_DIGIT_MASK));
        accumulator >>= BINARY_SHIFT;
        bits_in_accumulator -= BINARY_SHIFT;
      }
    }
    if (bits_in_accumulator) _digits.push_back(static_cast<Digit>(accumulator));
  }

  static Digit shift_digits_left(const Digit* input_digits,
                                 std::size_t input_digits_count,
                                 std::size_t shift, Digit* output_digits) {
    Digit accumulator = 0;
    for (std::size_t index = 0; index < input_digits_count; index++) {
      DoubleDigit step =
          (static_cast<DoubleDigit>(input_digits[index]) << shift) |
          accumulator;
      output_digits[index] = static_cast<Digit>(step) & BINARY_DIGIT_MASK;
      accumulator = static_cast<Digit>(step >> BINARY_SHIFT);
    }
    return accumulator;
  }

  static Digit shift_digits_right(const Digit* input_digits,
                                  std::size_t input_digits_count,
                                  std::size_t shift, Digit* output_digits) {
    Digit accumulator = 0;
    Digit mask = (static_cast<Digit>(1) << shift) - 1;
    for (std::size_t index = input_digits_count; index-- > 0;) {
      DoubleDigit step = static_cast<DoubleDigit>(accumulator) << BINARY_SHIFT |
                         input_digits[index];
      accumulator = static_cast<Digit>(step) & mask;
      output_digits[index] = static_cast<Digit>(step >> shift);
    }
    return accumulator;
  }

  double frexp(int& exponent) const {
    Digit result_digits[2 + (MANTISSA_BITS + 1) / BINARY_SHIFT] = {
        0,
    };
    static const int half_even_correction[8] = {0, -1, -2, 1, 0, -1, 2, 1};
    std::size_t size = _digits.size();
    std::size_t bits_count = to_bit_length(_digits.back());
    if (size >=
            (std::numeric_limits<std::size_t>::max() - 1) / BINARY_SHIFT + 1 &&
        (size >
             (std::numeric_limits<std::size_t>::max() - 1) / BINARY_SHIFT + 1 ||
         bits_count >
             (std::numeric_limits<std::size_t>::max() - 1) % BINARY_SHIFT + 1))
      throw std::overflow_error("Too large to convert to floating point.");
    bits_count = (size - 1) * BINARY_SHIFT + bits_count;
    std::size_t shift_digits, shift_bits, result_size;
    if (bits_count <= MANTISSA_BITS + 2) {
      shift_digits = (MANTISSA_BITS + 2 - bits_count) / BINARY_SHIFT;
      shift_bits = (MANTISSA_BITS + 2 - bits_count) % BINARY_SHIFT;
      result_size = shift_digits;
      Digit remainder = shift_digits_left(_digits.data(), size, shift_bits,
                                          result_digits + result_size);
      result_size += size;
      result_digits[result_size++] = remainder;
    } else {
      shift_digits = (bits_count - MANTISSA_BITS - 2) / BINARY_SHIFT;
      shift_bits = (bits_count - MANTISSA_BITS - 2) % BINARY_SHIFT;
      Digit remainder =
          shift_digits_right(_digits.data() + shift_digits, size - shift_digits,
                             shift_bits, result_digits);
      result_size = size - shift_digits;
      if (remainder)
        result_digits[0] |= 1;
      else
        while (shift_digits > 0)
          if (_digits[--shift_digits]) {
            result_digits[0] |= 1;
            break;
          }
    }
    result_digits[0] += half_even_correction[result_digits[0] & 7];
    double result_modulus = result_digits[--result_size];
    while (result_size > 0)
      result_modulus =
          result_modulus * BINARY_BASE + result_digits[--result_size];
    result_modulus /= 4.0 * MANTISSA_BITS_POWER_OF_TWO;
    if (result_modulus == 1.0) {
      if (bits_count == std::numeric_limits<std::size_t>::max())
        throw std::overflow_error("Too large to convert to floating point.");
      result_modulus = 0.5;
      bits_count += 1;
    }
    exponent = bits_count;
    if (exponent > std::numeric_limits<double>::max_exponent)
      throw std::overflow_error("Too large to convert to floating point.");
    return _sign * result_modulus;
  }

  void parse_non_binary_base_digits(const char* start, const char* stop,
                                    std::size_t base,
                                    std::size_t digits_count) {
    static double bases_logs[37] = {0.0};
    static std::size_t infimum_bases_exponents[37] = {0};
    static std::size_t infimum_bases_powers[37] = {0};
    if (bases_logs[base] == 0.0) {
      std::size_t infimum_base_power = base;
      std::size_t infimum_base_exponent = 1;
      bases_logs[base] = log(base) / log(BINARY_BASE);
      while (true) {
        std::size_t candidate = infimum_base_power * base;
        if (candidate > BINARY_BASE) break;
        infimum_base_power = candidate;
        ++infimum_base_exponent;
      }
      infimum_bases_powers[base] = infimum_base_power;
      infimum_bases_exponents[base] = infimum_base_exponent;
    }
    double digits_count_upper_bound = digits_count * bases_logs[base] + 1.0;
    if (digits_count_upper_bound >
        std::numeric_limits<std::size_t>::max() / sizeof(Digit))
      throw std::overflow_error("Too many digits.");
    _digits.reserve(static_cast<std::size_t>(digits_count_upper_bound));
    std::size_t infimum_base_exponent = infimum_bases_exponents[base];
    std::size_t infimum_base_power = infimum_bases_powers[base];
    while (start < stop) {
      if (*start == SEPARATOR) {
        ++start;
        continue;
      }
      DoubleDigit digit =
          static_cast<Digit>(ASCII_CODES_DIGIT_VALUES[mask_char(*(start++))]);
      std::size_t base_exponent = 1;
      for (; base_exponent < infimum_base_exponent && start != stop; ++start) {
        if (*start == SEPARATOR) continue;
        ++base_exponent;
        digit = static_cast<DoubleDigit>(
            digit * base + ASCII_CODES_DIGIT_VALUES[mask_char(*start)]);
      }
      std::size_t base_power = infimum_base_power;
      if (base_exponent != infimum_base_exponent)
        for (base_power = base; base_exponent > 1; --base_exponent)
          base_power *= base;
      for (std::size_t index = 0; index < _digits.size(); ++index) {
        digit += static_cast<DoubleDigit>(_digits[index]) * base_power;
        _digits[index] = static_cast<Digit>(digit & BINARY_DIGIT_MASK);
        digit >>= BINARY_SHIFT;
      }
      if (digit) _digits.push_back(digit);
    }
    if (_digits.empty()) _digits.push_back(0);
  }

  template <std::size_t BASE>
  std::vector<Digit> to_base_digits() const {
    if constexpr ((BASE & (BASE - 1)) == 0)
      return binary_digits_to_binary_base<Digit, Digit, BINARY_SHIFT,
                                          floor_log<2>(BASE)>(_digits);
    else
      return binary_digits_to_non_binary_base<Digit, Digit, BINARY_SHIFT, BASE>(
          _digits);
  }
};

#endif
