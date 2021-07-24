#ifndef INT_HPP
#define INT_HPP

#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
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

template <class Digit, std::size_t BINARY_SHIFT, char SEPARATOR>
class BigInt {
 public:
  static_assert(std::is_integral<Digit>() && std::is_unsigned<Digit>(),
                "Digits should be unsigned integrals.");
  static_assert(
      BINARY_SHIFT <= std::numeric_limits<Digit>::digits - 1,
      "Digit should be able to hold all integers lesser than double base.");
  static_assert(ASCII_CODES_DIGIT_VALUES[mask_char(SEPARATOR)] > 36,
                "Separator should not be a digit");

  using DoubleDigit = typename double_precision<Digit>::type;
  static_assert(!std::is_same<DoubleDigit, undefined>(),
                "Double precision version of digit type is undefined.");
  static_assert(std::is_integral<DoubleDigit>(),
                "Double precision digit should be integral.");
  static_assert(std::numeric_limits<DoubleDigit>::digits >= 2 * BINARY_SHIFT,
                "Double precision digit should be able to hold all integers "
                "lesser than squared base.");
  using SignedDigit = typename std::make_signed<Digit>::type;
  using SignedDoubleDigit = typename std::make_signed<DoubleDigit>::type;

  static constexpr Digit BINARY_BASE = 1 << BINARY_SHIFT;
  static constexpr Digit BINARY_DIGIT_MASK = BINARY_BASE - 1;
  static constexpr std::size_t DECIMAL_SHIFT = floor_log10<BINARY_BASE>();
  static constexpr std::size_t DECIMAL_BASE = power_of_ten<DECIMAL_SHIFT>();

  BigInt() : _sign(0), _digits({0}) {}

  BigInt(const char* characters, std::size_t base = 10) {
    if ((base != 0 && base < 2) || base > 36)
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

  BigInt operator+(const BigInt& other) const {
    if (_digits.size() == 1 && other._digits.size() == 1)
      return from_signed_digit(signed_digit() + other.signed_digit());
    if (_sign < 0) {
      if (other._sign < 0)
        return BigInt(-1, sum_moduli(_digits, other._digits));
      else {
        int sign = 1;
        std::vector<Digit> digits =
            subtract_moduli(other._digits, _digits, sign);
        return BigInt(sign, digits);
      }
    } else if (other._sign < 0) {
      int sign = 1;
      std::vector<Digit> digits = subtract_moduli(_digits, other._digits, sign);
      return BigInt(sign, digits);
    } else
      return BigInt(_sign | other._sign, sum_moduli(_digits, other._digits));
  }

  operator bool() const { return bool(_sign); }

  BigInt operator*(const BigInt& other) const {
    return _digits.size() == 1 && other._digits.size() == 1
               ? from_signed_double_digit(signed_double_digit() *
                                          other.signed_double_digit())
               : BigInt(_sign * other._sign,
                        multiply_digits(_digits, other._digits));
  }

  BigInt operator-() const { return BigInt(-_sign, _digits); }

  BigInt operator-(const BigInt& other) const {
    if (_digits.size() == 1 && other._digits.size() == 1)
      return from_signed_digit(signed_digit() - other.signed_digit());
    if (_sign < 0) {
      if (other._sign < 0) {
        int sign = 1;
        std::vector<Digit> digits =
            subtract_moduli(other._digits, _digits, sign);
        return BigInt(sign, digits);
      } else
        return BigInt(-1, sum_moduli(_digits, other._digits));
    } else if (other._sign < 0)
      return BigInt(1, sum_moduli(_digits, other._digits));
    else {
      int sign = _sign | other._sign;
      std::vector<Digit> digits = subtract_moduli(_digits, other._digits, sign);
      return BigInt(sign, digits);
    }
  }

  bool operator==(const BigInt& other) const {
    return _sign == other._sign && _digits == other._digits;
  }

  std::string repr(std::size_t base = 10) const {
    const std::vector<Digit> decimal_digits = to_decimal_digits();
    std::size_t characters_count = (_sign < 0) +
                                   (decimal_digits.size() - 1) * DECIMAL_SHIFT +
                                   floor_log10(decimal_digits.back()) + 1;
    char* characters = new char[characters_count + 1]();
    char* stop = &characters[characters_count];
    for (std::size_t index = 0; index < decimal_digits.size() - 1; index++) {
      Digit remainder = decimal_digits[index];
      for (std::size_t step = 0; step < DECIMAL_SHIFT; step++) {
        *--stop = '0' + (remainder % 10);
        remainder /= 10;
      }
    }
    for (Digit remainder = decimal_digits.back(); remainder != 0;
         remainder /= 10)
      *--stop = '0' + (remainder % 10);
    if (_sign == 0)
      *--stop = '0';
    else if (_sign < 0)
      *--stop = '-';
    return std::string(characters, characters_count);
  }

 private:
  int _sign;
  std::vector<Digit> _digits;

  static constexpr Digit KARATSUBA_CUTOFF = 1;
  static constexpr Digit KARATSUBA_SQUARE_CUTOFF = KARATSUBA_CUTOFF * 2;

  BigInt(int sign, const std::vector<Digit>& digits)
      : _sign(sign), _digits(digits) {}

  static BigInt from_signed_digit(SignedDigit value) {
    Digit modulus;
    int sign;
    if (value < 0) {
      modulus = -static_cast<Digit>(value);
      sign = -1;
    } else {
      modulus = static_cast<Digit>(value);
      sign = value == 0 ? 0 : 1;
    }
    Digit remainder = modulus >> BINARY_SHIFT;
    return remainder
               ? BigInt(sign, std::vector<Digit>(
                                  {modulus & BINARY_DIGIT_MASK, remainder}))
               : BigInt(sign, std::vector<Digit>({modulus}));
  }

  static BigInt from_signed_double_digit(SignedDoubleDigit value) {
    DoubleDigit modulus;
    int sign;
    if (value < 0) {
      sign = -1;
      modulus = static_cast<DoubleDigit>(-1 - value) + 1;
    } else if (value > 0) {
      sign = 1;
      modulus = static_cast<DoubleDigit>(value);
    } else
      return BigInt();
    DoubleDigit accumulator = modulus;
    std::vector<Digit> digits;
    while (accumulator) {
      digits.push_back(static_cast<Digit>(accumulator & BINARY_DIGIT_MASK));
      accumulator >>= BINARY_SHIFT;
    }
    return BigInt(sign, digits);
  }

  static Digit subtract_digits_in_place(Digit* longest,
                                        std::size_t size_longest,
                                        Digit* shortest,
                                        std::size_t size_shortest) {
    Digit accumulator = 0;
    std::size_t index = 0;
    for (; index < size_shortest; ++index) {
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

  static std::vector<Digit> subtract_moduli(const std::vector<Digit>& first,
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
                                   Digit* shortest, std::size_t size_shortest) {
    Digit accumulator = 0;
    std::size_t index = 0;
    for (; index < size_shortest; ++index) {
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

  static std::vector<Digit> sum_moduli(const std::vector<Digit>& first,
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
    if (size_shortest > size_longest) {
      std::swap(shortest, longest);
      std::swap(size_shortest, size_longest);
    }
    if (size_shortest <=
        (shortest == longest ? KARATSUBA_SQUARE_CUTOFF : KARATSUBA_CUTOFF)) {
      return size_shortest == 0 ? std::vector<Digit>({0})
                                : multiply_digits_plain(*shortest, *longest);
    }
    if (2 * size_shortest <= size_longest)
      return multiply_digits_lopsided(*shortest, *longest);
    std::size_t shift = size_longest >> 1;
    std::vector<Digit> shortest_high, shortest_low;
    split_digits(*shortest, shift, shortest_high, shortest_low);
    std::vector<Digit> longest_high, longest_low;
    if (shortest == longest) {
      longest_high = shortest_high;
      longest_low = shortest_low;
    } else
      split_digits(*longest, shift, longest_high, longest_low);
    std::vector<Digit> result(size_shortest + size_longest, 0);
    std::vector<Digit> highs_product =
        multiply_digits(shortest_high, longest_high);
    std::copy(highs_product.begin(), highs_product.end(),
              result.begin() + 2 * shift);
    std::size_t uninitialized_digits_count =
        result.size() - 2 * shift - highs_product.size();
    std::fill(result.begin() + 2 * shift + highs_product.size(),
              result.begin() + uninitialized_digits_count, 0);
    std::vector<Digit> lows_product =
        multiply_digits(shortest_low, longest_low);
    std::copy(lows_product.begin(), lows_product.end(), result.begin());
    uninitialized_digits_count = 2 * shift - lows_product.size();
    std::fill(result.begin() + lows_product.size(),
              result.begin() + uninitialized_digits_count, 0);
    std::size_t digits_after_shift = result.size() - shift;
    (void)subtract_digits_in_place(result.data() + shift, digits_after_shift,
                                   lows_product.data(), lows_product.size());
    (void)subtract_digits_in_place(result.data() + shift, digits_after_shift,
                                   highs_product.data(), highs_product.size());
    std::vector<Digit> shortest_components_sum =
        sum_moduli(shortest_high, shortest_low);
    std::vector<Digit> longest_components_sum;
    if (shortest == longest)
      longest_components_sum = shortest_components_sum;
    else
      longest_components_sum = sum_moduli(longest_high, longest_low);
    std::vector<Digit> components_sums_product =
        multiply_digits(shortest_components_sum, longest_components_sum);
    (void)sum_digits_in_place(result.data() + shift, digits_after_shift,
                              components_sums_product.data(),
                              components_sums_product.size());
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
      std::vector<Digit> step_digits(
          longest.begin() + processed_digits_count,
          longest.begin() + processed_digits_count + step_digits_count);
      std::vector<Digit> product = multiply_digits(shortest, step_digits);
      (void)sum_digits_in_place(result.data() + processed_digits_count,
                                result.size() - processed_digits_count,
                                product.data(), product.size());
      size_longest -= step_digits_count;
      processed_digits_count += step_digits_count;
    }
    normalize_digits(result);
    return result;
  }

  static std::vector<Digit> multiply_digits_plain(
      const std::vector<Digit>& first, const std::vector<Digit>& second) {
    std::size_t size_a = first.size(), size_b = second.size();
    std::vector<Digit> result(size_a + size_b, 0);
    if (&first == &second) {
      for (std::size_t index = 0; index < size_a; ++index) {
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
    } else {
      for (std::size_t index = 0; index < size_a; ++index) {
        DoubleDigit accumulator = 0;
        DoubleDigit digit = first[index];
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
    }
    normalize_digits(result);
    return result;
  }

  static void normalize_digits(std::vector<Digit>& digits) {
    std::size_t digits_count = digits.size();
    while (digits_count > 1 && digits[digits_count - 1] == 0) --digits_count;
    if (digits_count != digits.size()) digits.resize(digits_count);
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

  SignedDigit signed_digit() const {
    return _sign * static_cast<SignedDigit>(_digits[0]);
  }

  SignedDoubleDigit signed_double_digit() const {
    return _sign * static_cast<SignedDoubleDigit>(_digits[0]);
  }

  std::vector<Digit> to_decimal_digits() const {
    static const std::size_t scale = static_cast<std::size_t>(
        (33 * DECIMAL_SHIFT) / (10 * BINARY_SHIFT - 33 * DECIMAL_SHIFT));
    std::size_t digits_count = _digits.size();
    std::size_t result_digits_count =
        1 + digits_count + static_cast<std::size_t>(digits_count / scale);
    std::vector<Digit> result;
    result.reserve(result_digits_count);
    for (auto iterator = _digits.rbegin(); iterator != _digits.rend();
         ++iterator) {
      Digit digit = *iterator;
      for (std::size_t index = 0; index < result.size(); ++index) {
        DoubleDigit step =
            static_cast<DoubleDigit>(result[index]) << BINARY_SHIFT | digit;
        digit = static_cast<Digit>(step / DECIMAL_BASE);
        result[index] = static_cast<Digit>(
            step - static_cast<DoubleDigit>(digit) * DECIMAL_BASE);
      }
      while (digit) {
        result.push_back(digit % DECIMAL_BASE);
        digit /= DECIMAL_BASE;
      }
    }
    if (result.empty()) result.push_back(0);
    return result;
  }
};

#endif
