#ifndef INT_HPP
#define INT_HPP

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "digits.h"
#include "exceptions.h"
#include "utils.h"

namespace cppbuiltins {
constexpr char DIGIT_VALUES_ASCII_CODES[MAX_REPRESENTABLE_BASE] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

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
  static constexpr std::size_t BINARY_SHIFT = _BINARY_SHIFT;

  using DoubleDigit = DoublePrecisionOf<Digit>;
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

  template <class T,
            std::enable_if_t<std::is_unsigned_v<T> &&
                                 cppbuiltins::is_upcastable_v<T, Digit>,
                             int> = 0>
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
            std::enable_if_t<std::is_unsigned_v<T> &&
                                 !cppbuiltins::is_upcastable_v<T, Digit> &&
                                 cppbuiltins::is_upcastable_v<T, DoubleDigit>,
                             int> = 0>
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
            std::enable_if_t<std::is_signed_v<T> &&
                                 cppbuiltins::is_upcastable_v<T, SignedDigit>,
                             int> = 0>
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

  template <class T, std::enable_if_t<
                         std::is_signed_v<T> &&
                             !cppbuiltins::is_upcastable_v<T, SignedDigit> &&
                             cppbuiltins::is_upcastable_v<T, SignedDoubleDigit>,
                         int> = 0>
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

  explicit BigInt(const char* const characters, std::size_t base = 10) {
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
    _digits = parse_binary_digits<Digit, SEPARATOR, BINARY_SHIFT>(start, base);
    _sign *= (_digits.size() > 1 || _digits[0] != 0);
  }

  BigInt abs() const { return is_negative() ? BigInt(1, _digits) : *this; }

  BigInt bit_length() const {
    return _digits.size() <=
                   std::numeric_limits<std::size_t>::max() / BINARY_SHIFT
               ? BigInt((_digits.size() - 1) * BINARY_SHIFT +
                        cppbuiltins::bit_length(_digits.back()))
               : BigInt(_digits.size() - 1) * BigInt(BINARY_SHIFT) +
                     BigInt(cppbuiltins::bit_length(_digits.back()));
  }

  template <class Result = double,
            std::enable_if_t<std::is_floating_point_v<Result>, int> = 0>
  Result divide_approximately(const BigInt& divisor) const {
    using SignedSize = std::make_signed_t<std::size_t>;
    constexpr std::size_t MANTISSA_BITS = std::numeric_limits<Result>::digits;
    constexpr std::size_t MANTISSA_BINARY_DIGITS_COUNT =
        MANTISSA_BITS / BINARY_SHIFT;
    constexpr SignedSize MANTISSA_EXTRA_BITS = MANTISSA_BITS % BINARY_SHIFT;
    if (!divisor) throw ZeroDivisionError();
    bool negate = is_negative() ^ divisor.is_negative();
    if (!*this) return negate ? -0.0 : 0.0;
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
      Result reduced_dividend = reduce_digits<Result>(dividend_digits);
      Result reduced_divisor = reduce_digits<Result>(divisor_digits);
      Result result = reduced_dividend / reduced_divisor;
      return negate ? -result : result;
    }
    SignedSize digits_count_difference =
        dividend_digits_count - divisor_digits_count;
    if (digits_count_difference >
        static_cast<SignedSize>(std::numeric_limits<std::size_t>::max() /
                                BINARY_SHIFT) -
            1)
      throw std::overflow_error(
          "Division result too large to be expressed as floating point.");
    else if (digits_count_difference <
             1 - static_cast<SignedSize>(
                     std::numeric_limits<std::size_t>::max() / BINARY_SHIFT))
      return negate ? -0.0 : 0.0;
    SignedSize bit_lengths_difference =
        digits_count_difference * BINARY_SHIFT +
        (static_cast<SignedSize>(
             cppbuiltins::bit_length(dividend_digits.back())) -
         static_cast<SignedSize>(
             cppbuiltins::bit_length(divisor_digits.back())));
    if (bit_lengths_difference > std::numeric_limits<Result>::max_exponent)
      throw std::overflow_error(
          "Division result too large to be expressed as floating point.");
    else if (bit_lengths_difference <
             std::numeric_limits<Result>::min_exponent -
                 static_cast<SignedSize>(MANTISSA_BITS) - 1)
      return negate ? -0.0 : 0.0;
    SignedSize shift =
        std::max(bit_lengths_difference,
                 static_cast<SignedSize>(
                     std::numeric_limits<Result>::min_exponent)) -
        static_cast<SignedSize>(MANTISSA_BITS) - 2;
    bool inexact = false;
    Digit* quotient_data;
    std::size_t quotient_digits_count;
    if (shift <= 0) {
      std::size_t shift_digits =
          static_cast<std::size_t>(-shift) / BINARY_SHIFT;
      if (dividend_digits_count >=
          std::numeric_limits<SignedSize>::max() - 1 - shift_digits)
        throw std::overflow_error(
            "Division result too large to be expressed as floating point.");
      quotient_digits_count = dividend_digits_count + shift_digits + 1;
      quotient_data = new Digit[quotient_digits_count]();
      Digit remainder = shift_digits_left_in_place(
          dividend_digits.data(), dividend_digits_count,
          static_cast<std::size_t>(-shift) % BINARY_SHIFT,
          quotient_data + shift_digits);
      quotient_data[dividend_digits_count + shift_digits] = remainder;
    } else {
      std::size_t shift_digits = static_cast<std::size_t>(shift) / BINARY_SHIFT;
      quotient_digits_count = dividend_digits_count - shift_digits;
      quotient_data = new Digit[quotient_digits_count]();
      Digit remainder = shift_digits_right_in_place(
          dividend_digits.data() + shift_digits, quotient_digits_count,
          static_cast<std::size_t>(shift) % BINARY_SHIFT, quotient_data);
      if (remainder) inexact = true;
      while (!inexact && shift_digits > 0)
        if (dividend_digits[--shift_digits]) inexact = true;
    }
    std::vector<Digit> quotient_digits(quotient_data,
                                       quotient_data + quotient_digits_count);
    trim_leading_zeros(quotient_digits);
    if (divisor_digits_count == 1) {
      std::vector<Digit> next_quotient_digits;
      Digit remainder = divrem_digits_by_digit(
          quotient_digits, divisor_digits[0], next_quotient_digits);
      std::swap(quotient_digits, next_quotient_digits);
      if (remainder) inexact = true;
    } else {
      std::vector<Digit> next_quotient_digits, remainder;
      divrem_two_or_more_digits(quotient_digits, divisor_digits,
                                next_quotient_digits, remainder);
      std::swap(quotient_digits, next_quotient_digits);
      if (remainder.size() > 1 || remainder[0] != 0) inexact = true;
    }
    SignedSize quotient_bit_length = static_cast<SignedSize>(
        (quotient_digits.size() - 1) * BINARY_SHIFT +
        cppbuiltins::bit_length(quotient_digits.back()));
    SignedSize extra_bits =
        std::max(quotient_bit_length,
                 std::numeric_limits<Result>::min_exponent - shift) -
        static_cast<SignedSize>(MANTISSA_BITS);
    const Digit mask = static_cast<Digit>(1) << (extra_bits - 1);
    Digit quotient_low_digit = quotient_digits[0] | inexact;
    if ((quotient_low_digit & mask) && (quotient_low_digit & (3U * mask - 1U)))
      quotient_low_digit += mask;
    quotient_digits[0] = quotient_low_digit & ~(2U * mask - 1U);
    Result reduced_quotient = reduce_digits<Result>(quotient_digits);
    if (shift + quotient_bit_length >=
            std::numeric_limits<Result>::max_exponent &&
        (shift + quotient_bit_length >
             std::numeric_limits<Result>::max_exponent ||
         reduced_quotient ==
             std::ldexp(1.0, static_cast<int>(quotient_bit_length))))
      throw std::overflow_error(
          "Division result too large to be expressed as floating point.");
    Result result = std::ldexp(reduced_quotient, static_cast<int>(shift));
    return negate ? -result : result;
  }

  BigInt floor_divide(const BigInt& divisor) const {
    BigInt result;
    divmod<true, false>(divisor, &result, nullptr);
    return result;
  }

  BigInt gcd(const BigInt& other) const {
    std::vector<Digit> largest_digits = _digits,
                       smallest_digits = other._digits;
    if (digits_lesser_than(largest_digits, smallest_digits))
      std::swap(largest_digits, smallest_digits);
    for (std::size_t largest_digits_count;
         (largest_digits_count = largest_digits.size()) > 2;) {
      const std::size_t smallest_digits_count = smallest_digits.size();
      if (smallest_digits_count == 1 && smallest_digits[0] == 0)
        return BigInt(1, largest_digits);
      const std::size_t highest_digit_bit_length =
          cppbuiltins::bit_length(largest_digits.back());
      SignedDoubleDigit largest_leading_bits =
          (static_cast<SignedDoubleDigit>(
               largest_digits[largest_digits_count - 1])
           << (2 * BINARY_SHIFT - highest_digit_bit_length)) |
          (static_cast<SignedDoubleDigit>(
               largest_digits[largest_digits_count - 2])
           << (BINARY_SHIFT - highest_digit_bit_length)) |
          static_cast<SignedDoubleDigit>(
              largest_digits[largest_digits_count - 3] >>
              highest_digit_bit_length);
      SignedDoubleDigit smallest_leading_bits =
          (smallest_digits_count >= largest_digits_count - 2
               ? static_cast<SignedDoubleDigit>(
                     smallest_digits[largest_digits_count - 3] >>
                     highest_digit_bit_length)
               : 0) |
          (smallest_digits_count >= largest_digits_count - 1
               ? static_cast<SignedDoubleDigit>(
                     smallest_digits[largest_digits_count - 2])
                     << (BINARY_SHIFT - highest_digit_bit_length)
               : 0) |
          (smallest_digits_count >= largest_digits_count
               ? static_cast<SignedDoubleDigit>(
                     smallest_digits[largest_digits_count - 1])
                     << (2 * BINARY_SHIFT - highest_digit_bit_length)
               : 0);
      SignedDoubleDigit first_coefficient = 1, second_coefficient = 0,
                        third_coefficient = 0, fourth_coefficient = 1;
      std::size_t iterations_count = 0;
      for (;; ++iterations_count) {
        if (third_coefficient == smallest_leading_bits) break;
        const SignedDoubleDigit scale =
            (largest_leading_bits + (first_coefficient - 1)) /
            (smallest_leading_bits - third_coefficient);
        const SignedDoubleDigit next_third_coefficient =
            second_coefficient + scale * fourth_coefficient;
        const SignedDoubleDigit next_smallest_leading_bits =
            largest_leading_bits - scale * smallest_leading_bits;
        if (next_third_coefficient > next_smallest_leading_bits) break;
        largest_leading_bits = smallest_leading_bits;
        smallest_leading_bits = next_smallest_leading_bits;
        const SignedDoubleDigit next_fourth_coefficient =
            first_coefficient + scale * third_coefficient;
        first_coefficient = fourth_coefficient;
        second_coefficient = third_coefficient;
        third_coefficient = next_third_coefficient;
        fourth_coefficient = next_fourth_coefficient;
      }
      if (iterations_count == 0) {
        if (smallest_digits_count == 1) {
          std::vector<Digit> quotient;
          const Digit remainder = divrem_digits_by_digit(
              largest_digits, smallest_digits[0], quotient);
          largest_digits = smallest_digits;
          smallest_digits = std::vector<Digit>({remainder});
        } else {
          std::vector<Digit> quotient, remainder;
          divrem_two_or_more_digits(largest_digits, smallest_digits, quotient,
                                    remainder);
          largest_digits = smallest_digits;
          smallest_digits = remainder;
        }
        continue;
      }
      if (iterations_count & 1) {
        first_coefficient = -first_coefficient;
        second_coefficient = -second_coefficient;
        third_coefficient = -third_coefficient;
        fourth_coefficient = -fourth_coefficient;
        std::swap(first_coefficient, second_coefficient);
        std::swap(third_coefficient, fourth_coefficient);
      }
      SignedDoubleDigit next_largest_accumulator = 0;
      SignedDoubleDigit next_smallest_accumulator = 0;
      std::size_t index = 0;
      std::vector<Digit> next_largest_digits, next_smallest_digits;
      next_largest_digits.reserve(largest_digits_count);
      next_smallest_digits.reserve(largest_digits_count);
      for (; index < smallest_digits_count; ++index) {
        next_largest_accumulator +=
            (first_coefficient * largest_digits[index]) -
            (second_coefficient * smallest_digits[index]);
        next_smallest_accumulator +=
            (fourth_coefficient * smallest_digits[index]) -
            (third_coefficient * largest_digits[index]);
        next_largest_digits.push_back(
            static_cast<Digit>(next_largest_accumulator & BINARY_DIGIT_MASK));
        next_smallest_digits.push_back(
            static_cast<Digit>(next_smallest_accumulator & BINARY_DIGIT_MASK));
        next_largest_accumulator >>= BINARY_SHIFT;
        next_smallest_accumulator >>= BINARY_SHIFT;
      }
      for (; index < largest_digits_count; ++index) {
        next_largest_accumulator += first_coefficient * largest_digits[index];
        next_smallest_accumulator -= third_coefficient * largest_digits[index];
        next_largest_digits.push_back(
            static_cast<Digit>(next_largest_accumulator & BINARY_DIGIT_MASK));
        next_smallest_digits.push_back(
            static_cast<Digit>(next_smallest_accumulator & BINARY_DIGIT_MASK));
        next_largest_accumulator >>= BINARY_SHIFT;
        next_smallest_accumulator >>= BINARY_SHIFT;
      }
      trim_leading_zeros(next_largest_digits);
      trim_leading_zeros(next_smallest_digits);
      largest_digits = next_largest_digits;
      smallest_digits = next_smallest_digits;
    }
    return BigInt(
        cppbuiltins::gcd(reduce_digits<DoubleDigit>(largest_digits),
                         reduce_digits<DoubleDigit>(smallest_digits)));
  }

  void divmod(const BigInt& divisor, BigInt& quotient,
              BigInt& remainder) const {
    divmod<true, true>(divisor, &quotient, &remainder);
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
      if (result.is_negative()) result = divisor + result;
      return result;
    }
    throw std::invalid_argument("Not invertible.");
  }

  bool is_negative() const noexcept { return _sign < 0; }

  bool is_one() const noexcept {
    return is_positive() && _digits.size() == 1 && _digits[0] == 1;
  }

  bool is_positive() const noexcept { return _sign > 0; }

  BigInt mod(const BigInt& divisor) const {
    BigInt result;
    divmod<false, true>(divisor, nullptr, &result);
    return result;
  }

  BigInt power(const BigInt& exponent) const {
    return power(exponent, NoModulus{});
  }

  BigInt power_modulo(const BigInt& exponent, const BigInt& modulus) const {
    return power(exponent, modulus);
  }

  template <std::size_t BASE = 10,
            std::size_t TARGET_SHIFT =
                (BINARY_BASE < BASE ? 1 : floor_log<BASE>(BINARY_BASE)),
            std::size_t TARGET_BASE = cppbuiltins::const_power(BASE,
                                                               TARGET_SHIFT)>
  std::string repr() const noexcept {
    static_assert(1 < BASE && BASE <= MAX_REPRESENTABLE_BASE,
                  "Base should be range from 2 to 36.");
    const std::vector<Digit> base_digits = to_base_digits<TARGET_BASE>();
    const std::size_t characters_count =
        (is_negative()) + (base_digits.size() - 1) * TARGET_SHIFT +
        floor_log<BASE>(base_digits.back()) + 1;
    char* characters = new char[characters_count + 1]();
    char* stop = &characters[characters_count];
    for (std::size_t index = 0; index < base_digits.size() - 1; index++) {
      Digit remainder = base_digits[index];
      for (std::size_t step = 0; step < TARGET_SHIFT; step++) {
        *--stop = DIGIT_VALUES_ASCII_CODES[remainder % BASE];
        remainder /= BASE;
      }
    }
    for (Digit remainder = base_digits.back(); remainder != 0;
         remainder /= BASE)
      *--stop = DIGIT_VALUES_ASCII_CODES[remainder % BASE];
    if (!*this)
      *--stop = '0';
    else if (is_negative())
      *--stop = '-';
    std::string result(characters, characters_count);
    delete[] characters;
    return result;
  }

  explicit operator bool() const noexcept { return bool(_sign); }

  explicit operator double() const {
    if (_digits.size() == 1) return static_cast<double>(signed_digit());
    int exponent;
    double fraction = frexp<double>(exponent);
    return std::ldexp(fraction, exponent);
  }

  explicit operator float() const {
    if (_digits.size() == 1) return static_cast<float>(signed_digit());
    int exponent;
    float fraction = frexp<float>(exponent);
    return std::ldexp(fraction, exponent);
  }

  BigInt operator+(const BigInt& other) const noexcept {
    if (_digits.size() == 1 && other._digits.size() == 1)
      return BigInt(signed_digit() + other.signed_digit());
    if (is_negative()) {
      if (other.is_negative())
        return BigInt(-1, sum_digits(_digits, other._digits));
      else {
        Sign sign{1};
        std::vector<Digit> digits =
            subtract_digits(other._digits, _digits, sign);
        return BigInt(sign, digits);
      }
    } else if (other.is_negative()) {
      Sign sign{1};
      std::vector<Digit> digits = subtract_digits(_digits, other._digits, sign);
      return BigInt(sign, digits);
    } else
      return BigInt(_sign | other._sign, sum_digits(_digits, other._digits));
  }

  BigInt operator&(const BigInt& other) const noexcept {
    Sign sign;
    const auto digits = _digits.size() > other._digits.size()
                            ? bitwise_and_digits(_digits, _sign, other._digits,
                                                 other._sign, sign)
                            : bitwise_and_digits(other._digits, other._sign,
                                                 _digits, _sign, sign);
    return BigInt(sign, digits);
  }

  BigInt operator*(const BigInt& other) const noexcept {
    return _digits.size() == 1 && other._digits.size() == 1
               ? BigInt(signed_double_digit() * other.signed_double_digit())
               : BigInt(_sign * other._sign,
                        multiply_digits(_digits, other._digits));
  }

  BigInt operator-() const noexcept { return BigInt(-_sign, _digits); }

  BigInt operator~() const noexcept {
    if (_digits.size() == 1) return BigInt(-signed_digit() - 1);
    if (is_positive())
      return BigInt(-1, sum_digits(_digits, {1}));
    else {
      Sign sign{1};
      std::vector<Digit> digits = subtract_digits(_digits, {1}, sign);
      return BigInt(sign, digits);
    }
  }

  BigInt operator-(const BigInt& other) const noexcept {
    if (_digits.size() == 1 && other._digits.size() == 1)
      return BigInt(signed_digit() - other.signed_digit());
    if (is_negative()) {
      if (other.is_negative()) {
        Sign sign{1};
        std::vector<Digit> digits =
            subtract_digits(other._digits, _digits, sign);
        return BigInt(sign, digits);
      } else
        return BigInt(-1, sum_digits(_digits, other._digits));
    } else if (other.is_negative())
      return BigInt(1, sum_digits(_digits, other._digits));
    else {
      Sign sign = _sign | other._sign;
      std::vector<Digit> digits = subtract_digits(_digits, other._digits, sign);
      return BigInt(sign, digits);
    }
  }

  BigInt operator|(const BigInt& other) const noexcept {
    Sign sign;
    const auto digits = _digits.size() > other._digits.size()
                            ? bitwise_or_digits(_digits, _sign, other._digits,
                                                other._sign, sign)
                            : bitwise_or_digits(other._digits, other._sign,
                                                _digits, _sign, sign);
    return BigInt(sign, digits);
  }

  BigInt operator^(const BigInt& other) const noexcept {
    Sign sign;
    const auto digits = _digits.size() > other._digits.size()
                            ? bitwise_xor_digits(_digits, _sign, other._digits,
                                                 other._sign, sign)
                            : bitwise_xor_digits(other._digits, other._sign,
                                                 _digits, _sign, sign);
    return BigInt(sign, digits);
  }

  bool operator==(const BigInt& other) const noexcept {
    return _sign == other._sign && _digits == other._digits;
  }

  bool operator<(const BigInt& other) const noexcept {
    return _sign < other._sign ||
           (_sign == other._sign &&
            (is_positive() ? digits_lesser_than(_digits, other._digits)
                           : digits_lesser_than(other._digits, _digits)));
  }

  bool operator<=(const BigInt& other) const noexcept {
    return _sign < other._sign ||
           (_sign == other._sign &&
            (is_positive()
                 ? digits_lesser_than_or_equal(_digits, other._digits)
                 : digits_lesser_than_or_equal(other._digits, _digits)));
  }

  bool operator>(const BigInt& other) const noexcept {
    return _sign > other._sign ||
           (_sign == other._sign &&
            (is_positive() ? digits_lesser_than(other._digits, _digits)
                           : digits_lesser_than(_digits, other._digits)));
  }

  bool operator>=(const BigInt& other) const noexcept {
    return _sign > other._sign ||
           (_sign == other._sign &&
            (is_positive()
                 ? digits_lesser_than_or_equal(other._digits, _digits)
                 : digits_lesser_than_or_equal(_digits, other._digits)));
  }

  BigInt operator/(const BigInt& divisor) const {
    BigInt result;
    divrem<true, false>(divisor, &result, nullptr);
    return result;
  }

  BigInt operator%(const BigInt& divisor) const {
    BigInt result;
    divrem<false, true>(divisor, nullptr, &result);
    return result;
  }

  BigInt operator<<(const BigInt& shift) const {
    if (shift.is_negative()) {
      throw std::invalid_argument("Shift by negative step is undefined.");
    } else if (!*this) {
      return *this;
    } else {
      std::vector<Digit> shift_quotient_digits;
      Digit shift_remainder = divrem_digits_by_digit(
          shift._digits, static_cast<Digit>(BINARY_SHIFT),
          shift_quotient_digits);
      const std::size_t shift_quotient = safe_reduce_digits<std::size_t>(
          shift_quotient_digits, MAX_DIGITS_COUNT);
      if (shift_quotient >= MAX_DIGITS_COUNT)
        throw std::overflow_error("Too large shift step.");
      else
        return BigInt(
            _sign, shift_digits_left(_digits, shift_quotient, shift_remainder));
    }
  }

  BigInt operator>>(const BigInt& shift) const {
    if (shift.is_negative()) {
      throw std::invalid_argument("Shift by negative step is undefined.");
    } else if (!*this) {
      return *this;
    } else {
      std::vector<Digit> shift_quotient_digits;
      Digit shift_remainder = divrem_digits_by_digit(
          shift._digits, static_cast<Digit>(BINARY_SHIFT),
          shift_quotient_digits);
      const std::size_t shift_quotient = safe_reduce_digits<std::size_t>(
          shift_quotient_digits, MAX_DIGITS_COUNT);
      if (shift_quotient >= MAX_DIGITS_COUNT)
        return this->is_negative() ? ~BigInt() : BigInt();
      else if (this->is_negative()) {
        const auto inverted = ~*this;
        const auto digits = shift_digits_right(inverted._digits, shift_quotient,
                                               shift_remainder);
        return ~BigInt(inverted._sign * (digits.size() > 1 || digits[0] != 0),
                       digits);
      } else {
        const auto digits =
            shift_digits_right(_digits, shift_quotient, shift_remainder);
        return BigInt(_sign * (digits.size() > 1 || digits[0] != 0), digits);
      }
    }
  }

 protected:
  BigInt(Sign sign, const std::vector<Digit>& digits)
      : _sign(sign), _digits(digits) {}

  const std::vector<Digit>& digits() const noexcept { return _digits; }

  SignedDigit signed_digit() const noexcept {
    return _sign * static_cast<SignedDigit>(_digits[0]);
  }

  SignedDoubleDigit signed_double_digit() const noexcept {
    return _sign * static_cast<SignedDoubleDigit>(_digits[0]);
  }

 private:
  Sign _sign;
  std::vector<Digit> _digits;

  static constexpr std::size_t MAX_DIGITS_COUNT =
      std::numeric_limits<std::size_t>::max() / sizeof(Digit);
  static constexpr std::size_t WINDOW_CUTOFF = 8;
  static constexpr std::size_t WINDOW_SHIFT = 5;
  static constexpr std::size_t WINDOW_BASE = 1 << WINDOW_SHIFT;
  using WindowDigit = std::uint8_t;
  static_assert(WINDOW_SHIFT <= std::numeric_limits<WindowDigit>::digits,
                "Window digit type should be able to contain window digits.");

  static std::vector<Digit> bitwise_and_digits(std::vector<Digit> longest,
                                               const Sign longest_sign,
                                               std::vector<Digit> shortest,
                                               const Sign shortest_sign,
                                               Sign& sign) noexcept {
    if (longest_sign < 0) longest = complement_digits(std::move(longest));
    if (shortest_sign < 0) shortest = complement_digits(std::move(shortest));
    const std::size_t result_size =
        shortest_sign < 0 ? longest.size() : shortest.size();
    std::vector<Digit> result;
    result.reserve(result_size);
    for (std::size_t index = 0; index < shortest.size(); ++index)
      result.push_back(longest[index] & shortest[index]);
    for (std::size_t index = shortest.size(); index < result_size; ++index)
      result.push_back(longest[index]);
    sign = longest_sign & shortest_sign;
    if (sign < 0) {
      result.push_back(BINARY_DIGIT_MASK);
      result = complement_digits(std::move(result));
    }
    trim_leading_zeros(result);
    sign *= (result.size() > 1 || result[0] != 0);
    return result;
  }

  static std::vector<Digit> bitwise_or_digits(std::vector<Digit> longest,
                                              const Sign longest_sign,
                                              std::vector<Digit> shortest,
                                              const Sign shortest_sign,
                                              Sign& sign) noexcept {
    if (longest_sign < 0) longest = complement_digits(std::move(longest));
    if (shortest_sign < 0) shortest = complement_digits(std::move(shortest));
    const std::size_t result_size =
        shortest_sign < 0 ? shortest.size() : longest.size();
    std::vector<Digit> result;
    result.reserve(result_size);
    for (std::size_t index = 0; index < shortest.size(); ++index)
      result.push_back(longest[index] | shortest[index]);
    for (std::size_t index = shortest.size(); index < result_size; ++index)
      result.push_back(longest[index]);
    sign = longest_sign | shortest_sign;
    if (sign < 0) {
      result.push_back(BINARY_DIGIT_MASK);
      result = complement_digits(std::move(result));
    }
    trim_leading_zeros(result);
    return result;
  }

  static std::vector<Digit> bitwise_xor_digits(std::vector<Digit> longest,
                                               const Sign longest_sign,
                                               std::vector<Digit> shortest,
                                               const Sign shortest_sign,
                                               Sign& sign) noexcept {
    if (longest_sign < 0) longest = complement_digits(std::move(longest));
    if (shortest_sign < 0) shortest = complement_digits(std::move(shortest));
    std::vector<Digit> result = std::move(longest);
    for (std::size_t index = 0; index < shortest.size(); ++index)
      result[index] ^= shortest[index];
    if (shortest_sign < 0)
      for (std::size_t index = shortest.size(); index < result.size(); ++index)
        result[index] ^= BINARY_DIGIT_MASK;
    const bool sign_is_negative = (longest_sign < 0) ^ (shortest_sign < 0);
    if (sign_is_negative) {
      result.push_back(BINARY_DIGIT_MASK);
      result = complement_digits(std::move(result));
    }
    trim_leading_zeros(result);
    sign = (sign_is_negative ? -1 : 1) * (result.size() > 1 || result[0] != 0);
    return result;
  }

  static std::vector<Digit> complement_digits(
      const std::vector<Digit>& digits) noexcept {
    std::vector<Digit> result;
    result.reserve(digits.size());
    Digit accumulator = 1;
    for (const auto digit : digits) {
      accumulator += digit ^ BINARY_DIGIT_MASK;
      result.push_back(accumulator & BINARY_DIGIT_MASK);
      accumulator >>= BINARY_SHIFT;
    }
    assert(accumulator == 0);
    return result;
  }

  static void divrem_two_or_more_digits(
      const std::vector<Digit>& dividend, const std::vector<Digit>& divisor,
      std::vector<Digit>& quotient, std::vector<Digit>& remainder) noexcept {
    std::size_t dividend_digits_count = dividend.size();
    const std::size_t divisor_digits_count = divisor.size();
    Digit* const dividend_normalized = new Digit[dividend_digits_count + 1]();
    Digit* const divisor_normalized = new Digit[divisor_digits_count]();
    const std::size_t shift =
        BINARY_SHIFT - cppbuiltins::bit_length(divisor.back());
    shift_digits_left_in_place(divisor.data(), divisor_digits_count, shift,
                               divisor_normalized);
    Digit accumulator = shift_digits_left_in_place(
        dividend.data(), dividend_digits_count, shift, dividend_normalized);
    if (accumulator != 0 || dividend_normalized[dividend_digits_count - 1] >=
                                divisor_normalized[divisor_digits_count - 1])
      dividend_normalized[dividend_digits_count++] = accumulator;
    const std::size_t quotient_size =
        dividend_digits_count - divisor_digits_count;
    Digit* const quotient_data = new Digit[quotient_size]();
    const Digit last_divisor_digit_normalized =
        divisor_normalized[divisor_digits_count - 1];
    const Digit penult_divisor_digit_normalized =
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
        const SignedDoubleDigit step =
            static_cast<SignedDigit>(digits_normalized_tail[index]) +
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
    trim_leading_zeros(quotient);
    shift_digits_right_in_place(dividend_normalized, divisor_digits_count,
                                shift, divisor_normalized);
    delete[] dividend_normalized;
    remainder = std::vector<Digit>(divisor_normalized,
                                   divisor_normalized + divisor_digits_count);
    delete[] divisor_normalized;
    trim_leading_zeros(remainder);
  }

  static Digit divrem_digits_by_digit(const std::vector<Digit>& dividend,
                                      Digit divisor,
                                      std::vector<Digit>& quotient) noexcept {
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
    trim_leading_zeros(quotient);
    return static_cast<Digit>(remainder);
  }

  template <class Result,
            std::enable_if_t<std::is_floating_point_v<Result>, int> = 0>
  Result frexp(int& exponent) const {
    constexpr std::size_t MANTISSA_BITS = std::numeric_limits<Result>::digits;
    constexpr Result MANTISSA_BITS_POWER_OF_TWO =
        cppbuiltins::const_power(static_cast<Result>(2), MANTISSA_BITS);
    Digit result_digits[2 + (MANTISSA_BITS + 1) / BINARY_SHIFT] = {
        0,
    };
    std::size_t size = _digits.size();
    std::size_t bits_count = cppbuiltins::bit_length(_digits.back());
    if (size >=
            (std::numeric_limits<std::size_t>::max() - 1) / BINARY_SHIFT + 1 &&
        (size >
             (std::numeric_limits<std::size_t>::max() - 1) / BINARY_SHIFT + 1 ||
         bits_count >
             (std::numeric_limits<std::size_t>::max() - 1) % BINARY_SHIFT + 1))
      throw std::overflow_error("Too large to convert to floating point.");
    bits_count += (size - 1) * BINARY_SHIFT;
    std::size_t result_size;
    if (bits_count <= MANTISSA_BITS + 2) {
      const std::size_t shift_digits =
          (MANTISSA_BITS + 2 - bits_count) / BINARY_SHIFT;
      const std::size_t shift_bits =
          (MANTISSA_BITS + 2 - bits_count) % BINARY_SHIFT;
      result_size = shift_digits;
      const Digit remainder = shift_digits_left_in_place(
          _digits.data(), size, shift_bits, result_digits + result_size);
      result_size += size;
      result_digits[result_size++] = remainder;
    } else {
      std::size_t shift_digits =
          (bits_count - MANTISSA_BITS - 2) / BINARY_SHIFT;
      const std::size_t shift_bits =
          (bits_count - MANTISSA_BITS - 2) % BINARY_SHIFT;
      const Digit remainder = shift_digits_right_in_place(
          _digits.data() + shift_digits, size - shift_digits, shift_bits,
          result_digits);
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
    constexpr SignedDigit HALF_EVEN_CORRECTION[8] = {0, -1, -2, 1, 0, -1, 2, 1};
    result_digits[0] =
        static_cast<Digit>(static_cast<SignedDigit>(result_digits[0]) +
                           HALF_EVEN_CORRECTION[result_digits[0] & 7]);
    Result result_modulus = result_digits[--result_size];
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
    exponent = static_cast<int>(bits_count);
    if (exponent > std::numeric_limits<double>::max_exponent)
      throw std::overflow_error("Too large to convert to floating point.");
    return _sign * result_modulus;
  }

  template <class Result,
            std::enable_if_t<std::is_arithmetic_v<Result>, int> = 0>
  static Result reduce_digits(const std::vector<Digit>& digits) noexcept {
    Result result = 0;
    for (auto position = digits.rbegin(); position != digits.rend(); ++position)
      if constexpr (std::is_integral_v<Result>)
        result = (result << BINARY_SHIFT) | *position;
      else
        result = (result * BINARY_BASE) + *position;
    return result;
  }

  template <class Result, std::enable_if_t<std::is_integral_v<Result>, int> = 0>
  static Result safe_reduce_digits(const std::vector<Digit>& digits,
                                   const Result fallback) noexcept {
    Result candidate = 0;
    for (auto position = digits.rbegin(); position != digits.rend();
         ++position) {
      const auto shifted = candidate << BINARY_SHIFT;
      if (shifted < candidate) return fallback;
      candidate = shifted | *position;
    }
    return candidate;
  }

  static Digit subtract_digits_in_place(
      Digit* longest, std::size_t size_longest,
      const std::vector<Digit>& shortest) noexcept {
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
                                            Sign& sign) noexcept {
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
    trim_leading_zeros(result);
    return result;
  }

  static Digit sum_digits_in_place(
      Digit* longest, std::size_t size_longest,
      const std::vector<Digit>& shortest) noexcept {
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

  static std::vector<Digit> sum_digits(
      const std::vector<Digit>& first,
      const std::vector<Digit>& second) noexcept {
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
    trim_leading_zeros(result);
    return result;
  }

  static void split_digits(const std::vector<Digit>& digits, std::size_t size,
                           std::vector<Digit>& high,
                           std::vector<Digit>& low) noexcept {
    const std::size_t size_low = std::min<std::size_t>(digits.size(), size);
    const typename std::vector<Digit>::const_iterator mid =
        digits.begin() + size_low;
    low = std::vector<Digit>(digits.begin(), mid);
    high = std::vector<Digit>(mid, digits.end());
    trim_leading_zeros(high);
    trim_leading_zeros(low);
  }

  static std::vector<Digit> multiply_digits(
      const std::vector<Digit>& first,
      const std::vector<Digit>& second) noexcept {
    const std::vector<Digit>*shortest = &first, *longest = &second;
    std::size_t size_shortest = shortest->size(),
                size_longest = longest->size();
    if (size_longest < size_shortest) {
      std::swap(shortest, longest);
      std::swap(size_shortest, size_longest);
    }
    static constexpr std::size_t KARATSUBA_CUTOFF = 70;
    static constexpr std::size_t KARATSUBA_SQUARE_CUTOFF = KARATSUBA_CUTOFF * 2;
    if (size_shortest <=
        ((shortest == longest) ? KARATSUBA_SQUARE_CUTOFF : KARATSUBA_CUTOFF)) {
      return size_shortest == 1 && (*shortest)[0] == 0
                 ? std::vector<Digit>({0})
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
    trim_leading_zeros(result);
    return result;
  }

  static std::vector<Digit> multiply_digits_lopsided(
      const std::vector<Digit>& shortest,
      const std::vector<Digit>& longest) noexcept {
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
    trim_leading_zeros(result);
    return result;
  }

  static std::vector<Digit> multiply_digits_plain(
      const std::vector<Digit>& first,
      const std::vector<Digit>& second) noexcept {
    std::size_t first_size = first.size(), second_size = second.size();
    std::vector<Digit> result(first_size + second_size, 0);
    if (&first == &second)
      for (std::size_t index = 0; index < first_size; ++index) {
        DoubleDigit digit = first[index];
        auto result_position = result.begin() + (index << 1);
        auto first_position = first.begin() + (index + 1);
        DoubleDigit accumulator = *result_position + digit * digit;
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
    trim_leading_zeros(result);
    return result;
  }

  static std::vector<Digit> shift_digits_left(
      const std::vector<Digit>& digits, const std::size_t shift_quotient,
      const std::size_t shift_remainder) noexcept {
    std::vector<Digit> result;
    result.reserve(shift_quotient + !!(shift_remainder) + digits.size());
    for (std::size_t index = 0; index < shift_quotient; ++index)
      result.push_back(0);
    DoubleDigit accumulator = 0;
    for (const auto digit : digits) {
      accumulator |= static_cast<DoubleDigit>(digit) << shift_remainder;
      result.push_back(static_cast<Digit>(accumulator & BINARY_DIGIT_MASK));
      accumulator >>= BINARY_SHIFT;
    }
    if (shift_remainder != 0) result.push_back(static_cast<Digit>(accumulator));
    trim_leading_zeros(result);
    return result;
  }

  static Digit shift_digits_left_in_place(const Digit* input_digits,
                                          std::size_t input_digits_count,
                                          std::size_t shift,
                                          Digit* output_digits) noexcept {
    Digit accumulator = 0;
    for (std::size_t index = 0; index < input_digits_count; index++) {
      DoubleDigit step =
          (static_cast<DoubleDigit>(input_digits[index]) << shift) |
          accumulator;
      output_digits[index] = static_cast<Digit>(step & BINARY_DIGIT_MASK);
      accumulator = static_cast<Digit>(step >> BINARY_SHIFT);
    }
    return accumulator;
  }

  static std::vector<Digit> shift_digits_right(
      const std::vector<Digit>& digits, std::size_t shift_quotient,
      const Digit shift_remainder) noexcept {
    if (digits.size() <= shift_quotient) return std::vector<Digit>({0});
    const std::size_t result_digits_count = digits.size() - shift_quotient;
    const std::size_t high_shift =
        BINARY_SHIFT - static_cast<std::size_t>(shift_remainder);
    const Digit low_mask = (1 << high_shift) - 1;
    const Digit high_mask = BINARY_DIGIT_MASK ^ low_mask;
    std::vector<Digit> result(result_digits_count);
    std::size_t position = shift_quotient;
    for (std::size_t index = 0; index < result_digits_count; ++index) {
      result[index] = (digits[position] >> shift_remainder) & low_mask;
      if (index + 1 < result_digits_count)
        result[index] |= (digits[position + 1] << high_shift) & high_mask;
      position += 1;
    }
    trim_leading_zeros(result);
    return result;
  }

  static Digit shift_digits_right_in_place(const Digit* input_digits,
                                           std::size_t input_digits_count,
                                           std::size_t shift,
                                           Digit* output_digits) noexcept {
    Digit accumulator = 0;
    DoubleDigit mask = (static_cast<DoubleDigit>(1) << shift) - 1;
    for (std::size_t index = input_digits_count; index-- > 0;) {
      DoubleDigit step = static_cast<DoubleDigit>(accumulator) << BINARY_SHIFT |
                         input_digits[index];
      accumulator = static_cast<Digit>(step & mask);
      output_digits[index] = static_cast<Digit>(step >> shift);
    }
    return accumulator;
  }

  template <bool WITH_QUOTIENT, bool WITH_REMAINDER>
  void divmod(const BigInt& divisor, BigInt* quotient,
              BigInt* remainder) const {
    static_assert(WITH_QUOTIENT || WITH_REMAINDER,
                  "Quotient or remainder or both should be requested.");
    std::size_t digits_count = _digits.size(),
                divisor_digits_count = divisor._digits.size();
    if (!divisor)
      throw ZeroDivisionError();
    else if (!*this) {
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
      Sign remainder_sign = _sign;
      if (divisor_digits_count == 1) {
        std::vector<Digit> quotient_digits;
        Digit remainder_digit = divrem_digits_by_digit(
            _digits, divisor._digits[0], quotient_digits);
        remainder_sign *= remainder_digit != 0;
        if constexpr (WITH_QUOTIENT)
          *quotient = BigInt(_sign * divisor._sign, quotient_digits);
        if constexpr (WITH_REMAINDER)
          *remainder =
              BigInt(remainder_sign, std::vector<Digit>{remainder_digit});
      } else {
        std::vector<Digit> quotient_digits, remainder_digits;
        divrem_two_or_more_digits(_digits, divisor._digits, quotient_digits,
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

  template <bool WITH_QUOTIENT, bool WITH_REMAINDER>
  void divrem(const BigInt& divisor, BigInt* quotient,
              BigInt* remainder) const {
    static_assert(WITH_QUOTIENT || WITH_REMAINDER,
                  "Quotient or remainder or both should be requested.");
    std::size_t digits_count = _digits.size(),
                divisor_digits_count = divisor._digits.size();
    if (!divisor)
      throw ZeroDivisionError();
    else if (!*this) {
      if constexpr (WITH_QUOTIENT) *quotient = BigInt();
      if constexpr (WITH_REMAINDER) *remainder = *this;
    } else if (digits_count < divisor_digits_count ||
               (digits_count == divisor_digits_count &&
                _digits.back() < divisor._digits.back())) {
      if constexpr (WITH_QUOTIENT) *quotient = BigInt();
      if constexpr (WITH_REMAINDER) *remainder = *this;
    } else {
      Sign remainder_sign = _sign;
      if (divisor_digits_count == 1) {
        std::vector<Digit> quotient_digits;
        Digit remainder_digit = divrem_digits_by_digit(
            _digits, divisor._digits[0], quotient_digits);
        remainder_sign *= remainder_digit != 0;
        if constexpr (WITH_QUOTIENT)
          *quotient = BigInt(_sign * divisor._sign, quotient_digits);
        if constexpr (WITH_REMAINDER)
          *remainder =
              BigInt(remainder_sign, std::vector<Digit>{remainder_digit});
      } else {
        std::vector<Digit> quotient_digits, remainder_digits;
        divrem_two_or_more_digits(_digits, divisor._digits, quotient_digits,
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
    }
  }

  using NoModulus = nullptr_t;

  const BigInt& mod(NoModulus) const noexcept { return *this; }

  template <class Modulus>
  BigInt power(BigInt exponent, Modulus modulus) const {
    BigInt base = *this;
    bool is_negative = false;
    if constexpr (!std::is_same<Modulus, NoModulus>()) {
      if (!modulus) throw std::invalid_argument("Modulus cannot be zero.");
      is_negative = modulus.is_negative();
      modulus = modulus.abs();
      if (modulus.is_one()) return BigInt();
      if (exponent.is_negative()) {
        exponent = -exponent;
        base = base.invmod(modulus);
      }
      if (base.is_negative() || base.digits().size() > modulus.digits().size())
        base = base.mod(modulus);
    } else if (exponent.is_negative())
      throw std::range_error(
          "Either exponent should be positive or modulus should be specified.");
    const std::vector<Digit>& exponent_digits = exponent.digits();
    Digit exponent_digit = exponent_digits.back();
    std::size_t exponent_digits_count = exponent_digits.size();
    BigInt result = BigInt(1u);
    if (exponent_digits_count == 1 && exponent_digit <= 3) {
      if (exponent_digit >= 2) {
        result = (base * base).mod(modulus);
        if (exponent_digit == 3) result = (result * base).mod(modulus);
      } else if (exponent_digit == 1)
        result = (base * result).mod(modulus);
    } else if (exponent_digits_count <= WINDOW_CUTOFF) {
      result = base;
      Digit exponent_mask = 2;
      for (;; exponent_mask <<= 1)
        if (exponent_mask > exponent_digit) {
          exponent_mask >>= 1;
          break;
        }
      exponent_mask >>= 1;
      for (auto exponent_digit_position = exponent_digits.rbegin();;) {
        for (; exponent_mask != 0; exponent_mask >>= 1) {
          result = (result * result).mod(modulus);
          if (exponent_digit & exponent_mask)
            result = (result * base).mod(modulus);
        }
        if (++exponent_digit_position == exponent_digits.rend()) break;
        exponent_digit = *exponent_digit_position;
        exponent_mask = static_cast<Digit>(1) << (BINARY_SHIFT - 1);
      }
    } else {
      BigInt cache[WINDOW_BASE];
      cache[0] = result;
      for (std::size_t index = 1; index < WINDOW_BASE; ++index)
        cache[index] = (cache[index - 1] * base).mod(modulus);
      std::vector<WindowDigit> exponent_window_digits =
          binary_digits_to_binary_base<Digit, WindowDigit, BINARY_SHIFT,
                                       WINDOW_SHIFT>(exponent_digits);
      for (auto exponent_digit_position = exponent_window_digits.rbegin();
           exponent_digit_position != exponent_window_digits.rend();
           ++exponent_digit_position) {
        const WindowDigit digit = *exponent_digit_position;
        for (std::size_t iteration = 0; iteration < WINDOW_SHIFT; ++iteration)
          result = (result * result).mod(modulus);
        if (digit) result = (result * cache[digit]).mod(modulus);
      }
    }
    if constexpr (!std::is_same<Modulus, NoModulus>()) {
      if (is_negative && result) result = result - modulus;
    }
    return result;
  }

  template <std::size_t BASE>
  std::vector<Digit> to_base_digits() const noexcept {
    if constexpr ((BASE & (BASE - 1)) == 0)
      return binary_digits_to_binary_base<Digit, Digit, BINARY_SHIFT,
                                          floor_log<2>(BASE)>(_digits);
    else
      return binary_digits_to_non_binary_base<Digit, Digit, BINARY_SHIFT, BASE>(
          _digits);
  }
};
}  // namespace cppbuiltins

#endif
