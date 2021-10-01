#ifndef DIGITS_HPP
#define DIGITS_HPP

#include <algorithm>
#include <vector>

#include "utils.h"

namespace cppbuiltins {

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

static constexpr unsigned char mask_char(char character) {
  return static_cast<unsigned char>(character & 0xff);
}

static constexpr bool is_space(char character) {
  return ASCII_CODES_WHITESPACE_FLAGS[mask_char(character)];
}

template <class SourceDigit, class TargetDigit, std::size_t TARGET_SHIFT,
          TargetDigit TARGET_DIGIT_MASK =
              cppbuiltins::const_power(TargetDigit(2), TARGET_SHIFT) - 1>
static std::vector<TargetDigit> binary_digits_to_greater_binary_base(
    const std::vector<SourceDigit>& source, std::size_t source_shift) {
  const std::size_t result_digits_count = static_cast<std::size_t>(
      (source.size() * TARGET_SHIFT + TARGET_SHIFT - 1) / TARGET_SHIFT);
  std::vector<TargetDigit> result;
  result.reserve(result_digits_count);
  DoublePrecisionOf<TargetDigit> accumulator = 0;
  std::size_t accumulator_bits_count = 0;
  for (const SourceDigit digit : source) {
    accumulator |= static_cast<DoublePrecisionOf<TargetDigit>>(digit)
                   << accumulator_bits_count;
    accumulator_bits_count += source_shift;
    if (accumulator_bits_count >= TARGET_SHIFT) {
      result.push_back(
          static_cast<TargetDigit>(accumulator & TARGET_DIGIT_MASK));
      accumulator >>= TARGET_SHIFT;
      accumulator_bits_count -= TARGET_SHIFT;
    }
  }
  if (accumulator || result.empty()) result.push_back(accumulator);
  return result;
}

template <class SourceDigit, class TargetDigit, std::size_t SOURCE_SHIFT,
          std::size_t TARGET_SHIFT>
static std::vector<TargetDigit> binary_digits_to_greater_binary_base(
    const std::vector<SourceDigit>& source) {
  static_assert(SOURCE_SHIFT < TARGET_SHIFT,
                "Target base should be greater than a source one.");
  return binary_digits_to_greater_binary_base<SourceDigit, TargetDigit,
                                              TARGET_SHIFT>(source,
                                                            SOURCE_SHIFT);
}

template <class SourceDigit, class TargetDigit, std::size_t TARGET_SHIFT,
          std::size_t TARGET_DIGIT_MASK = (TargetDigit(1) << TARGET_SHIFT) - 1>
static std::vector<TargetDigit> binary_digits_to_lesser_binary_base(
    const std::vector<SourceDigit>& source, std::size_t source_shift) {
  const std::size_t result_digits_bits_count =
      ((source.size() - 1) * source_shift + bit_length(source.back()));
  const std::size_t result_digits_count = static_cast<std::size_t>(
      (result_digits_bits_count + (TARGET_SHIFT - 1)) / TARGET_SHIFT);
  std::vector<TargetDigit> result;
  result.reserve(result_digits_count);
  DoublePrecisionOf<SourceDigit> accumulator = source[0];
  for (std::size_t accumulator_bits_count = source_shift, index = 1;
       index < source.size(); ++index, accumulator_bits_count += source_shift) {
    do {
      result.push_back(
          static_cast<TargetDigit>(accumulator & TARGET_DIGIT_MASK));
      accumulator >>= TARGET_SHIFT;
      accumulator_bits_count -= TARGET_SHIFT;
    } while (accumulator_bits_count >= TARGET_SHIFT);
    accumulator |= static_cast<DoublePrecisionOf<SourceDigit>>(source[index])
                   << accumulator_bits_count;
  }
  do {
    result.push_back(static_cast<TargetDigit>(accumulator & TARGET_DIGIT_MASK));
    accumulator >>= TARGET_SHIFT;
  } while (accumulator != 0);
  return result;
}

template <class SourceDigit, class TargetDigit, std::size_t SOURCE_SHIFT,
          std::size_t TARGET_SHIFT>
static std::vector<TargetDigit> binary_digits_to_lesser_binary_base(
    const std::vector<SourceDigit>& source) {
  static_assert(SOURCE_SHIFT > TARGET_SHIFT,
                "Target base should be lesser than a source one.");
  return binary_digits_to_lesser_binary_base<SourceDigit, TargetDigit,
                                             TARGET_SHIFT>(source,
                                                           SOURCE_SHIFT);
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
static std::vector<TargetDigit> binary_digits_to_non_binary_base(
    const std::vector<SourceDigit>& source) {
  std::size_t result_max_digits_count =
      1 + static_cast<std::size_t>(source.size() * SOURCE_SHIFT /
                                   std::log2(TARGET_BASE));
  std::vector<TargetDigit> result;
  result.reserve(result_max_digits_count);
  using Digit = std::conditional_t<(std::numeric_limits<SourceDigit>::digits <
                                    std::numeric_limits<TargetDigit>::digits),
                                   TargetDigit, SourceDigit>;
  for (auto iterator = source.rbegin(); iterator != source.rend(); ++iterator) {
    Digit digit = *iterator;
    for (std::size_t index = 0; index < result.size(); ++index) {
      DoublePrecisionOf<TargetDigit> step =
          (static_cast<DoublePrecisionOf<TargetDigit>>(result[index])
           << SOURCE_SHIFT) |
          digit;
      digit = step / TARGET_BASE;
      result[index] = static_cast<TargetDigit>(
          step -
          static_cast<DoublePrecisionOf<TargetDigit>>(digit) * TARGET_BASE);
    }
    while (digit) {
      result.push_back(digit % TARGET_BASE);
      digit /= TARGET_BASE;
    }
  }
  if (result.empty()) result.push_back(0);
  return result;
}

template <class SourceDigit, class TargetDigit, std::size_t TARGET_SHIFT,
          std::size_t TARGET_BASE = const_power(2, TARGET_SHIFT),
          std::size_t TARGET_DIGIT_MASK = TARGET_BASE - 1>
static std::vector<TargetDigit> non_binary_digits_to_greater_binary_base(
    const std::vector<SourceDigit>& source, std::size_t source_base) {
  double result_digits_count_upper_bound =
      source.size() * log(source_base) / log(TARGET_BASE) + 1.0;
  if (result_digits_count_upper_bound >
      std::numeric_limits<std::size_t>::max() / sizeof(TargetDigit))
    throw std::overflow_error("Too many digits.");
  std::vector<TargetDigit> result;
  result.reserve(static_cast<std::size_t>(result_digits_count_upper_bound));
  std::size_t infimum_base_power = source_base;
  std::size_t infimum_base_exponent = 1;
  while (true) {
    std::size_t candidate = infimum_base_power * source_base;
    if (candidate > TARGET_BASE) break;
    infimum_base_power = candidate;
    ++infimum_base_exponent;
  }
  for (auto position = source.rbegin(); position != source.rend();) {
    DoublePrecisionOf<TargetDigit> digit =
        static_cast<DoublePrecisionOf<TargetDigit>>(*(position++));
    std::size_t base_exponent = 1;
    for (; base_exponent < infimum_base_exponent && position != source.rend();
         ++base_exponent, ++position) {
      digit = DoublePrecisionOf<TargetDigit>(digit * source_base + *position);
    }
    std::size_t base_power = infimum_base_power;
    if (base_exponent != infimum_base_exponent)
      for (base_power = source_base; base_exponent > 1; --base_exponent)
        base_power *= source_base;
    for (std::size_t index = 0; index < result.size(); ++index) {
      digit += static_cast<DoublePrecisionOf<TargetDigit>>(result[index]) *
               base_power;
      result[index] = static_cast<TargetDigit>(digit & TARGET_DIGIT_MASK);
      digit >>= TARGET_SHIFT;
    }
    if (digit) result.push_back(digit);
  }
  if (result.empty()) result.push_back(0);
  return result;
}

template <class SourceDigit, class TargetDigit, std::size_t TARGET_SHIFT,
          std::size_t TARGET_BASE = const_power(2, TARGET_SHIFT),
          std::size_t TARGET_DIGIT_MASK = TARGET_BASE - 1>
static std::vector<TargetDigit> non_binary_digits_to_lesser_binary_base(
    const std::vector<SourceDigit>& source, std::size_t source_base) {
  double result_digits_count_upper_bound =
      source.size() * log(source_base) / log(TARGET_BASE) + 1.0;
  if (result_digits_count_upper_bound >
      std::numeric_limits<std::size_t>::max() / sizeof(TargetDigit))
    throw std::overflow_error("Too many digits.");
  std::vector<TargetDigit> result;
  result.reserve(static_cast<std::size_t>(result_digits_count_upper_bound));
  for (auto position = source.rbegin(); position != source.rend(); ++position) {
    DoublePrecisionOf<TargetDigit> digit =
        static_cast<DoublePrecisionOf<TargetDigit>>(*position);
    for (std::size_t index = 0; index < result.size(); ++index) {
      digit += static_cast<DoublePrecisionOf<TargetDigit>>(result[index]) *
               source_base;
      result[index] = static_cast<TargetDigit>(digit & TARGET_DIGIT_MASK);
      digit >>= TARGET_SHIFT;
    }
    while (digit) {
      result.push_back(digit & TARGET_DIGIT_MASK);
      digit >>= TARGET_SHIFT;
    }
  }
  if (result.empty()) result.push_back(0);
  return result;
}

template <class Digit>
static void trim_leading_zeros(std::vector<Digit>& digits) {
  std::size_t digits_count = digits.size();
  while (digits_count > 1 && digits[digits_count - 1] == 0) --digits_count;
  if (digits_count != digits.size()) digits.resize(digits_count);
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

template <class Digit, std::size_t BINARY_SHIFT,
          std::size_t BINARY_BASE = const_power(2, BINARY_SHIFT)>
static std::vector<Digit> binary_digits_from_binary_base(
    const std::vector<unsigned char>& source, std::size_t source_shift) {
  if constexpr (BINARY_BASE >= MAX_REPRESENTABLE_BASE) {
    return binary_digits_to_greater_binary_base<unsigned char, Digit,
                                                BINARY_SHIFT>(source,
                                                              source_shift);
  } else {
    return (source_shift < BINARY_SHIFT)
               ? binary_digits_to_greater_binary_base<unsigned char, Digit,
                                                      BINARY_SHIFT>(
                     source, source_shift)
               : binary_digits_to_lesser_binary_base<unsigned char, Digit,
                                                     BINARY_SHIFT>(
                     source, source_shift);
  }
}

template <class Digit, std::size_t BINARY_SHIFT,
          std::size_t BINARY_BASE = const_power(2, BINARY_SHIFT)>
static std::vector<Digit> binary_digits_from_non_binary_base(
    const std::vector<unsigned char>& source, std::size_t source_base) {
  if constexpr (BINARY_BASE >= MAX_REPRESENTABLE_BASE) {
    return non_binary_digits_to_greater_binary_base<unsigned char, Digit,
                                                    BINARY_SHIFT>(source,
                                                                  source_base);
  } else {
    return source_base < BINARY_BASE
               ? non_binary_digits_to_greater_binary_base<unsigned char, Digit,
                                                          BINARY_SHIFT>(
                     source, source_base)
               : non_binary_digits_to_lesser_binary_base<unsigned char, Digit,
                                                         BINARY_SHIFT>(
                     source, source_base);
  }
}

template <char SEPARATOR>
static std::vector<unsigned char> parse_digits(const char* const start,
                                               const char* stop,
                                               std::size_t digits_count) {
  std::vector<unsigned char> result;
  result.reserve(digits_count);
  while (start < stop--) {
    if (*stop != SEPARATOR) {
      result.push_back(ASCII_CODES_DIGIT_VALUES[mask_char(*stop)]);
    }
  }
  return result;
}

template <class Digit, char SEPARATOR, std::size_t BINARY_SHIFT>
static std::vector<Digit> parse_binary_digits(const char* start,
                                              std::size_t base) {
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
  while (*stop != '\0' && (ASCII_CODES_DIGIT_VALUES[mask_char(*stop)] < base ||
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
  while ((*start == '0' && start + 1 < stop) || *start == SEPARATOR) {
    ++start;
  }
  std::vector<unsigned char> digits =
      parse_digits<SEPARATOR>(start, stop, digits_count);
  std::vector<Digit> result =
      (base & (base - 1))
          ? binary_digits_from_non_binary_base<Digit, BINARY_SHIFT>(digits,
                                                                    base)
          : binary_digits_from_binary_base<Digit, BINARY_SHIFT>(
                digits, floor_log2(base));
  return result;
}
}  // namespace cppbuiltins

#endif
