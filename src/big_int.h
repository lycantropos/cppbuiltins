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
      BINARY_SHIFT <= std::numeric_limits<Digit>::digits - 2,
      "Digit should be able to hold all integers lesser than quadruple base.");
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

  static constexpr Digit BINARY_BASE = 1 << BINARY_SHIFT;
  static constexpr Digit BINARY_DIGIT_MASK = BINARY_BASE - 1;
  static constexpr std::size_t DECIMAL_SHIFT = floor_log10<BINARY_BASE>();
  static constexpr std::size_t DECIMAL_BASE = power_of_ten<DECIMAL_SHIFT>();

  BigInt() : _sign(0), _digits({0}) {}

  explicit BigInt(SignedDigit value) {
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

  BigInt<Digit, BINARY_SHIFT, SEPARATOR> operator+(
      const BigInt<Digit, BINARY_SHIFT, SEPARATOR>& other) const {
    if (_digits.size() == 1 && other._digits.size() == 1)
      return BigInt<Digit, BINARY_SHIFT, SEPARATOR>(
          _sign * static_cast<SignedDigit>(_digits[0]) +
          other._sign * static_cast<SignedDigit>(other._digits[0]));
    if (_sign < 0) {
      if (other._sign < 0)
        return sum_moduli(other);
      else
        return other.subtract_moduli(*this);
    } else if (other._sign < 0)
      return subtract_moduli(other);
    else
      return sum_moduli(other);
  }

  operator bool() const { return bool(_sign); }

  BigInt<Digit, BINARY_SHIFT, SEPARATOR> operator-() const {
    return BigInt<Digit, BINARY_SHIFT, SEPARATOR>(-_sign, _digits);
  }

  BigInt<Digit, BINARY_SHIFT, SEPARATOR> operator-(
      const BigInt<Digit, BINARY_SHIFT, SEPARATOR>& other) const {
    if (_digits.size() == 1 && other._digits.size() == 1)
      return BigInt<Digit, BINARY_SHIFT, SEPARATOR>(
          _sign * static_cast<SignedDigit>(_digits[0]) -
          other._sign * static_cast<SignedDigit>(other._digits[0]));
    if (_sign < 0) {
      if (other._sign < 0)
        return other.subtract_moduli(*this);
      else
        return sum_moduli(other);
    } else if (other._sign < 0)
      return sum_moduli(other);
    else
      return subtract_moduli(other);
  }

  bool operator==(const BigInt<Digit, BINARY_SHIFT, SEPARATOR>& other) const {
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

  BigInt(int sign, const std::vector<Digit>& digits)
      : _sign(sign), _digits(digits) {}

  BigInt<Digit, BINARY_SHIFT, SEPARATOR> subtract_moduli(
      const BigInt<Digit, BINARY_SHIFT, SEPARATOR>& other) const {
    const BigInt<Digit, BINARY_SHIFT, SEPARATOR>*longest = this,
                                      *shortest = &other;
    std::size_t size_longest = longest->_digits.size(),
                size_shortest = shortest->_digits.size();
    int swapping_sign = 1;
    Digit accumulator = 0;
    if (size_longest < size_shortest) {
      swapping_sign = -1;
      std::swap(longest, shortest);
      std::swap(size_longest, size_shortest);
    } else if (size_longest == size_shortest) {
      std::size_t index = size_shortest;
      while (--index > 0 && longest->_digits[index] == shortest->_digits[index])
        ;
      if (index == 0 && longest->_digits[0] == shortest->_digits[0])
        return BigInt<Digit, BINARY_SHIFT, SEPARATOR>();
      if (longest->_digits[index] < shortest->_digits[index]) {
        swapping_sign = -1;
        std::swap(longest, shortest);
      }
      size_longest = size_shortest = index + 1;
    }
    std::vector<Digit> digits;
    digits.reserve(size_longest);
    std::size_t index = 0;
    for (; index < size_shortest; ++index) {
      accumulator =
          longest->_digits[index] - shortest->_digits[index] - accumulator;
      digits.push_back(accumulator & BINARY_DIGIT_MASK);
      accumulator >>= BINARY_SHIFT;
      accumulator &= 1;
    }
    for (; index < size_longest; ++index) {
      accumulator = longest->_digits[index] - accumulator;
      digits.push_back(accumulator & BINARY_DIGIT_MASK);
      accumulator >>= BINARY_SHIFT;
      accumulator &= 1;
    }
    normalize_digits(digits);
    return BigInt<Digit, BINARY_SHIFT, SEPARATOR>(_sign * swapping_sign,
                                                  digits);
  }

  BigInt<Digit, BINARY_SHIFT, SEPARATOR> sum_moduli(
      const BigInt<Digit, BINARY_SHIFT, SEPARATOR>& other) const {
    const BigInt *longest = this, *shortest = &other;
    std::size_t size_longest = longest->_digits.size(),
                size_shortest = shortest->_digits.size();
    if (size_longest < size_shortest) {
      std::swap(size_longest, size_shortest);
      std::swap(longest, shortest);
    }
    std::vector<Digit> digits;
    digits.reserve(size_longest + 1);
    Digit accumulator = 0;
    std::size_t index = 0;
    for (; index < size_shortest; ++index) {
      accumulator += longest->_digits[index] + shortest->_digits[index];
      digits.push_back(accumulator & BINARY_DIGIT_MASK);
      accumulator >>= BINARY_SHIFT;
    }
    for (; index < size_longest; ++index) {
      accumulator += longest->_digits[index];
      digits.push_back(accumulator & BINARY_DIGIT_MASK);
      accumulator >>= BINARY_SHIFT;
    }
    digits.push_back(accumulator);
    normalize_digits(digits);
    return BigInt<Digit, BINARY_SHIFT, SEPARATOR>(_sign, digits);
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
          static_cast<Digit>(ASCII_CODES_DIGIT_VALUES[mask_char(*start++)]);
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
