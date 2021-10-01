#ifndef FRACTION_HPP
#define FRACTION_HPP

#include <type_traits>
#include <utility>

#include "exceptions.h"
#include "utils.h"

namespace cppbuiltins {
template <class Number>
class Gcd {
 public:
  Number operator()(ConstParameterFrom<Number> first,
                    ConstParameterFrom<Number> second) const {
    return gcd(first, second);
  }
};

template <class Component, class Gcd = Gcd<Component>>
class Fraction {
 public:
  Fraction() : _numerator(Component()), _denominator(Component(1)) {}

  explicit Fraction(ConstParameterFrom<Component> numerator,
                    ConstParameterFrom<Component> denominator = Component(1))
      : Fraction(numerator, denominator, std::true_type{}) {}

  Fraction operator+(const Fraction& other) const {
    return Fraction(
        _numerator * other._denominator + _denominator * other._numerator,
        _denominator * other._denominator);
  }

  Fraction operator+(ConstParameterFrom<Component> other) const {
    return Fraction(_numerator + _denominator * other, _denominator);
  }

  explicit operator bool() const { return bool(_numerator); }

  explicit operator double() const {
    return cppbuiltins::divide_as_double<Component>(_numerator, _denominator);
  }

  explicit operator Component() const {
    return is_negative() ? ceil() : floor();
  }

  bool operator==(const Fraction& other) const {
    return _numerator == other._numerator && _denominator == other._denominator;
  }

  bool operator==(ConstParameterFrom<Component> other) const {
    return cppbuiltins::is_one<Component>(_denominator) && _numerator == other;
  }

  bool operator<(const Fraction& other) const {
    return _numerator * other._denominator < _denominator * other._numerator;
  }

  bool operator<(ConstParameterFrom<Component> other) const {
    return _numerator < _denominator * other;
  }

  bool operator<=(const Fraction& other) const {
    return _numerator * other._denominator <= _denominator * other._numerator;
  }

  bool operator<=(ConstParameterFrom<Component> other) const {
    return _numerator <= _denominator * other;
  }

  bool operator>(const Fraction& other) const {
    return _numerator * other._denominator > _denominator * other._numerator;
  }

  bool operator>(ConstParameterFrom<Component> other) const {
    return _numerator > _denominator * other;
  }

  bool operator>=(const Fraction& other) const {
    return _numerator * other._denominator >= _denominator * other._numerator;
  }

  bool operator>=(ConstParameterFrom<Component> other) const {
    return _numerator >= _denominator * other;
  }

  Fraction operator%(const Fraction& other) const {
    return Fraction(
        (_numerator * other._denominator) % (other._numerator * _denominator),
        _denominator * other._denominator);
  }

  Fraction operator%(ConstParameterFrom<Component> other) const {
    return Fraction(_numerator % (other * _denominator), _denominator);
  }

  Fraction operator*(const Fraction& other) const {
    ConstParameterFrom<Component> numerator_other_denominator_gcd =
        gcd(_numerator, other._denominator);
    ConstParameterFrom<Component> other_numerator_denominator_gcd =
        gcd(_denominator, other._numerator);
    return Fraction((_numerator / numerator_other_denominator_gcd) *
                        (other._numerator / other_numerator_denominator_gcd),
                    (_denominator / other_numerator_denominator_gcd) *
                        (other._denominator / numerator_other_denominator_gcd),
                    std::false_type{});
  }

  Fraction operator*(ConstParameterFrom<Component> other) const {
    ConstParameterFrom<Component> denominator_other_gcd =
        gcd(_denominator, other);
    return Fraction(_numerator * (other / denominator_other_gcd),
                    _denominator / denominator_other_gcd, std::false_type{});
  }

  Fraction operator-() const {
    return Fraction(-_numerator, _denominator, std::false_type{});
  }

  const Fraction& operator+() const { return *this; }

  Fraction operator-(const Fraction& other) const {
    return Fraction(
        _numerator * other._denominator - _denominator * other._numerator,
        _denominator * other._denominator);
  }

  Fraction operator-(ConstParameterFrom<Component> other) const {
    return Fraction(_numerator - _denominator * other, _denominator);
  }

  Fraction operator/(const Fraction& other) const {
    ConstParameterFrom<Component> numerators_gcd =
        gcd(_numerator, other._numerator);
    ConstParameterFrom<Component> denominators_gcd =
        gcd(_denominator, other._denominator);
    return Fraction(
        (_numerator / numerators_gcd) * (other._denominator / denominators_gcd),
        (other._numerator / numerators_gcd) *
            (_denominator / denominators_gcd));
  }

  Fraction operator/(ConstParameterFrom<Component> other) const {
    ConstParameterFrom<Component> numerators_gcd = gcd(_numerator, other);
    return Fraction(_numerator / numerators_gcd,
                    (other / numerators_gcd) * _denominator);
  }

  Component ceil() const { return -((-_numerator) / _denominator); }

  Component floor() const { return _numerator / _denominator; }

  ConstParameterFrom<Component>& denominator() const { return _denominator; }

  void divmod(const Fraction& divisor, Component& quotient,
              Fraction& remainder) const {
    quotient = floor_divide(divisor);
    remainder = operator%(divisor);
  }

  Component floor_divide(const Fraction& other) const {
    return (_numerator * other._denominator) /
           (other._numerator * _denominator);
  }

  Component floor_divide(ConstParameterFrom<Component> other) const {
    return _numerator / (other * _denominator);
  }

  ConstParameterFrom<Component>& numerator() const { return _numerator; }

  Fraction power(ConstParameterFrom<Component> exponent) const {
    if (cppbuiltins::is_negative<Component>(exponent)) {
      if (!*this) throw ZeroDivisionError();
      Component exponent_modulus = -exponent;
      return is_negative() ? Fraction(cppbuiltins::power<Component>(
                                          -_denominator, exponent_modulus),
                                      cppbuiltins::power<Component>(
                                          -_numerator, exponent_modulus),
                                      std::false_type{})
                           : Fraction(cppbuiltins::power<Component>(
                                          _denominator, exponent_modulus),
                                      cppbuiltins::power<Component>(
                                          _numerator, exponent_modulus),
                                      std::false_type{});
    }
    return Fraction(cppbuiltins::power<Component>(_numerator, exponent),
                    cppbuiltins::power<Component>(_denominator, exponent),
                    std::false_type{});
  }

  bool is_negative() const {
    return cppbuiltins::is_negative<Component>(_numerator);
  }

  bool is_positive() const {
    return cppbuiltins::is_positive<Component>(_numerator);
  }

 private:
  static const Gcd gcd;
  Component _numerator, _denominator;

  template <bool NORMALIZE>
  Fraction(ConstParameterFrom<Component>& numerator,
           ConstParameterFrom<Component>& denominator,
           std::bool_constant<NORMALIZE>)
      : _numerator(numerator), _denominator(denominator) {
    if constexpr (NORMALIZE) {
      if (!_denominator) throw ZeroDivisionError();
      if (cppbuiltins::is_negative<Component>(_denominator)) {
        _numerator = -_numerator;
        _denominator = -_denominator;
      }
      Component components_gcd = gcd(_numerator, _denominator);
      if (!cppbuiltins::is_one<Component>(components_gcd)) {
        _denominator = _denominator / components_gcd;
        _numerator = _numerator / components_gcd;
      }
    }
  }
};

template <class Component, class Gcd>
const Gcd Fraction<Component, Gcd>::gcd{};

template <class Component, class Gcd>
bool operator<(ConstParameterFrom<Component> left,
               const Fraction<Component, Gcd>& right) {
  return left * right.denominator() < right.numerator();
}

template <class Component, class Gcd>
bool operator<=(ConstParameterFrom<Component> left,
                const Fraction<Component, Gcd>& right) {
  return left * right.denominator() <= right.numerator();
}


template <class Component, class Gcd>
bool operator==(ConstParameterFrom<Component> left,
                const Fraction<Component, Gcd>& right) {
  return is_one<Component>(right.denominator()) && left == right.numerator();
}

template <class Component, class Gcd>
bool operator>(ConstParameterFrom<Component> left,
               const Fraction<Component, Gcd>& right) {
  return left * right.denominator() > right.numerator();
}

template <class Component, class Gcd>
bool operator>=(ConstParameterFrom<Component> left,
                const Fraction<Component, Gcd>& right) {
  return left * right.denominator() >= right.numerator();
}

}  // namespace cppbuiltins

#endif
