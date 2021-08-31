#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "big_int.h"
#include "utils.h"

namespace py = pybind11;

#ifndef Py_SET_SIZE
#define Py_SET_SIZE(object, size) ((PyVarObject*)(object))->ob_size = size
#endif

#define MODULE_NAME cppbuiltins
#define C_STR_HELPER(a) #a
#define C_STR(a) C_STR_HELPER(a)
#define FRACTION_NAME "Fraction"
#define INT_NAME "int"
#define LIST_ITERATOR_NAME "list_iterator"
#define LIST_NAME "list"
#define LIST_REVERSED_ITERATOR_NAME "list_reversed_iterator"
#define SET_ITERATOR_NAME "set_iterator"
#define SET_NAME "set"
#ifndef VERSION_INFO
#define VERSION_INFO "dev"
#endif

using Index = Py_ssize_t;
using IterableState = py::list;
using IteratorState = py::tuple;
using Object = py::object;
using RawList = std::vector<Object>;

template <>
struct std::hash<Object> {
  std::size_t operator()(const Object& object) const {
    auto result = PyObject_Hash(object.ptr());
    if (result == -1) {
      throw py::error_already_set();
    }
    return static_cast<std::size_t>(result);
  }
};

using RawSet = std::unordered_set<Object>;
using Size = size_t;
using TokenValue = bool;
using WrappedTokenValue = std::shared_ptr<TokenValue>;

class Token {
 public:
  Token(std::shared_ptr<WrappedTokenValue> container)
      : _value(*container), _container(std::move(container)) {}

  bool expired() const {
    return std::addressof(*_value) != std::addressof(**_container);
  }

 private:
  WrappedTokenValue _value;
  std::shared_ptr<WrappedTokenValue> _container;
};

class Tokenizer {
 public:
  Tokenizer()
      : _container(std::make_shared<WrappedTokenValue>(
            std::make_shared<TokenValue>())) {}

  void reset() { _container->reset(new TokenValue()); }

  Token create() const { return {_container}; }

 private:
  std::shared_ptr<WrappedTokenValue> _container;
};

template <class T>
T&& identity(T&& value) {
  return std::forward<T>(value);
}

static std::string object_to_repr(const Object& object) {
  return {py::repr(object)};
}

namespace pybind11 {
static std::ostream& operator<<(std::ostream& stream, const Object& object) {
  return stream << ::object_to_repr(object);
}

static bool operator==(const Object& left, const Object& right) {
  return left.equal(right);
}
}  // namespace pybind11

template <class Type>
static std::string to_repr(const Type& value) {
  std::ostringstream stream;
  stream.precision(std::numeric_limits<double>::digits10 + 2);
  stream << value;
  return {stream.str()};
}

using BaseInt =
    BigInt<std::conditional_t<sizeof(void*) == 8, std::uint32_t, std::uint16_t>,
           '_'>;

static int int_to_sign(const py::int_& value) {
  PyLongObject* ptr = (PyLongObject*)value.ptr();
  Py_ssize_t signed_size = Py_SIZE(ptr);
  return signed_size < 0 ? -1 : signed_size > 0;
}

static std::vector<BaseInt::Digit> int_to_digits(const py::int_& value) {
  PyLongObject* ptr = (PyLongObject*)value.ptr();
  Py_ssize_t signed_size = Py_SIZE(ptr);
  std::size_t size = Py_ABS(signed_size) + (signed_size == 0);
  return binary_digits_to_binary_base<digit, BaseInt::Digit, PyLong_SHIFT,
                                      BaseInt::BINARY_SHIFT>(
      std::vector<digit>(ptr->ob_digit, ptr->ob_digit + size));
}

static const char* pystr_to_ascii_c_str(const py::str& string) {
  py::str ascii_string = py::reinterpret_steal<py::str>(
      _PyUnicode_TransformDecimalAndSpaceToASCII(string.ptr()));
  if (!ascii_string) throw py::error_already_set();
  const char* result = PyUnicode_AsUTF8(ascii_string.ptr());
  if (!result) throw py::error_already_set();
  return result;
}

class Int : public BaseInt {
 public:
  Int() : BaseInt() {}

  explicit Int(const BaseInt& value) : BaseInt(value) {}

  explicit Int(py::int_ value)
      : BaseInt(int_to_sign(value), int_to_digits(value)) {}

  Int(const py::str& value, std::size_t base)
      : BaseInt(pystr_to_ascii_c_str(value), base) {}

  operator py::int_() const {
    return py::reinterpret_steal<py::int_>((PyObject*)as_PyLong());
  }

  Int operator%(const Int& divisor) const {
    try {
      return Int(BaseInt::operator%(divisor));
    } catch (const std::range_error& exception) {
      PyErr_SetString(PyExc_ZeroDivisionError, exception.what());
      throw py::error_already_set();
    }
  }

  const Int& operator+() const { return *this; }

  Int operator+(const Int& other) const {
    return Int(BaseInt::operator+(other));
  }

  Int operator~() const { return Int(BaseInt::operator~()); }

  Int operator*(const Int& other) const {
    return Int(BaseInt::operator*(other));
  }

  Int operator-() const { return Int(BaseInt::operator-()); }

  Int operator-(const Int& other) const {
    return Int(BaseInt::operator-(other));
  }

  double operator/(const BigInt& divisor) const {
    try {
      return BaseInt::operator/(divisor);
    } catch (const std::range_error& exception) {
      PyErr_SetString(PyExc_ZeroDivisionError, exception.what());
      throw py::error_already_set();
    }
  }

  Int abs() const { return Int(BaseInt::abs()); }

  PyLongObject* as_PyLong() const {
    int sign = this->sign();
    const std::vector<BaseInt::Digit>& digits = this->digits();
    std::vector<digit> result_digits = binary_digits_to_binary_base<
        BaseInt::Digit, digit, BaseInt::BINARY_SHIFT, PyLong_SHIFT>(digits);
    PyLongObject* result = _PyLong_New(result_digits.size());
    std::memcpy(result->ob_digit, result_digits.data(),
                sizeof(digit) * result_digits.size());
    Py_SET_SIZE(result, Py_SIZE(result) * sign);
    return result;
  }

  py::tuple divmod(const Int& divisor) const {
    Int quotient, remainder;
    try {
      BaseInt::divmod(divisor, quotient, remainder);
    } catch (const std::range_error& exception) {
      PyErr_SetString(PyExc_ZeroDivisionError, exception.what());
      throw py::error_already_set();
    }
    return py::make_tuple(quotient, remainder);
  }

  Int floor_divide(const Int& divisor) const {
    try {
      return Int(BaseInt::floor_divide(divisor));
    } catch (const std::range_error& exception) {
      PyErr_SetString(PyExc_ZeroDivisionError, exception.what());
      throw py::error_already_set();
    }
  }

  Int gcd(const Int& other) const { return Int(BaseInt::gcd(other)); }

  Py_hash_t hash() const {
    int sign = this->sign();
    const std::vector<BaseInt::Digit>& digits = this->digits();
    if (digits.size() == 1) {
      if (sign > 0)
        return digits[0];
      else if (sign < 0)
        return -static_cast<Py_hash_t>(digits[0] + (digits[0] == 1));
      else
        return 0;
    }
    Py_uhash_t result = 0;
    for (auto position = digits.rbegin(); position != digits.rend();
         ++position) {
      result = ((result << BaseInt::BINARY_SHIFT) & _PyHASH_MODULUS) |
               (result >> (_PyHASH_BITS - BaseInt::BINARY_SHIFT));
      result += *position;
      if (result >= _PyHASH_MODULUS) result -= _PyHASH_MODULUS;
    }
    result = result * sign;
    result -= (result == std::numeric_limits<Py_uhash_t>::max());
    return static_cast<Py_hash_t>(result);
  }

  Int invmod(const Int& divisor) const { return Int(BaseInt::invmod(divisor)); }

  bool is_one() const { return BaseInt::is_one(); }

  py::object pow(const Int& exponent,
                 const Int* maybe_modulus = nullptr) const {
    if (maybe_modulus == nullptr && exponent.sign() < 0) {
      PyObject* result = PyFloat_Type.tp_as_number->nb_power(
          (PyObject*)as_PyLong(), (PyObject*)exponent.as_PyLong(), Py_None);
      if (!result) throw py::error_already_set();
      return py::reinterpret_steal<py::object>(result);
    }
    return py::cast(Int(BaseInt::pow(exponent, maybe_modulus)));
  }

  int sign() const { return BaseInt::sign(); }
};

static std::ostream& operator<<(std::ostream& stream, const Int& value) {
  return stream << C_STR(MODULE_NAME) "." INT_NAME "('" << value.repr() << "')";
}

static py::object pow(const py::float_& base, const py::float_& exponent) {
  PyObject* result = PyNumber_Power(base.ptr(), exponent.ptr(), Py_None);
  if (!result) throw py::error_already_set();
  return py::reinterpret_steal<py::object>(result);
}

class Fraction {
 public:
  Fraction() : _numerator(Int()), _denominator(Int(1)) {}

  Fraction(const Int& numerator, const Int& denominator = Int(1))
      : Fraction(numerator, denominator, std::true_type{}) {}

  Fraction operator+(const Fraction& other) const {
    return Fraction(
        _numerator * other._denominator + _denominator * other._numerator,
        _denominator * other._denominator);
  }

  Fraction operator+(const Int& other) const {
    return Fraction(_numerator + _denominator * other, _denominator);
  }

  operator bool() const { return bool(_numerator); }

  operator double() const { return _numerator / _denominator; }

  bool operator==(const Fraction& other) const {
    return _numerator == other._numerator && _denominator == other._denominator;
  }

  bool operator<=(const Fraction& other) const {
    return sign() < other.sign() ||
           (sign() == other.sign() &&
            _numerator * other._denominator <= _denominator * other._numerator);
  }

  bool operator<(const Fraction& other) const {
    return sign() < other.sign() ||
           (sign() == other.sign() &&
            _numerator * other._denominator < _denominator * other._numerator);
  }

  Fraction operator%(const Fraction& other) const {
    return Fraction(
        (_numerator * other._denominator) % (other._numerator * _denominator),
        _denominator * other._denominator);
  }

  Fraction operator%(const Int& other) const {
    return Fraction(_numerator % (other * _denominator), _denominator);
  }

  Fraction operator*(const Fraction& other) const {
    const Int numerator_other_denominator_gcd =
        _numerator.gcd(other._denominator);
    const Int other_numerator_denominator_gcd =
        _denominator.gcd(other._numerator);
    return Fraction(
        _numerator.floor_divide(numerator_other_denominator_gcd) *
            other._numerator.floor_divide(other_numerator_denominator_gcd),
        _denominator.floor_divide(other_numerator_denominator_gcd) *
            other._denominator.floor_divide(numerator_other_denominator_gcd));
  }

  Fraction operator*(const Int& other) const {
    const Int denominator_other_gcd = _denominator.gcd(other);
    return Fraction(_numerator * other.floor_divide(denominator_other_gcd),
                    _denominator.floor_divide(denominator_other_gcd));
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

  Fraction operator-(const Int& other) const {
    return Fraction(_numerator - _denominator * other, _denominator);
  }

  Fraction operator/(const Fraction& other) const {
    const Int numerators_gcd = _numerator.gcd(other._numerator);
    const Int denominators_gcd = _denominator.gcd(other._denominator);
    return Fraction(_numerator.floor_divide(numerators_gcd) *
                        other._denominator.floor_divide(denominators_gcd),
                    other._numerator.floor_divide(numerators_gcd) *
                        _denominator.floor_divide(denominators_gcd));
  }

  Fraction operator/(const Int& other) const {
    const Int numerators_gcd = _numerator.gcd(other);
    return Fraction(_numerator.floor_divide(numerators_gcd),
                    other.floor_divide(numerators_gcd) * _denominator);
  }

  Int ceil() const { return -(-_numerator).floor_divide(_denominator); }

  Int floor() const { return _numerator.floor_divide(_denominator); }

  const Int& denominator() const { return _denominator; }

  py::tuple divmod(const Fraction& divisor) const {
    Int quotient = floor_divide(divisor);
    Fraction remainder = operator%(divisor);
    return py::make_tuple(quotient, remainder);
  }

  Int floor_divide(const Fraction& other) const {
    return (_numerator * other._denominator)
        .floor_divide(other._numerator * _denominator);
  }

  Int floor_divide(const Int& other) const {
    return _numerator.floor_divide(other * _denominator);
  }

  Py_hash_t hash() const {
    static const Int HASH_MODULUS{_PyHASH_MODULUS};
    Py_hash_t result;
    try {
      result =
          (Int(_numerator.abs().hash()) * _denominator.invmod(HASH_MODULUS))
              .hash();
    } catch (const std::invalid_argument&) {
      result = _PyHASH_INF;
    }
    if (_numerator.sign() < 0) result = -result;
    return result - (result == -1);
  }

  const Int& numerator() const { return _numerator; }

  Fraction pow(const Int& exponent) const {
    if (exponent.sign() < 0) {
      if (_numerator.sign() == 0) {
        PyErr_SetString(PyExc_ZeroDivisionError,
                        "Denominator should not be zero.");
        throw py::error_already_set();
      }
      Int exponent_modulus = -exponent;
      return _numerator.sign() < 0
                 ? Fraction(Int(static_cast<BaseInt>(-_denominator)
                                    .pow(exponent_modulus)),
                            Int(static_cast<BaseInt>(-_numerator)
                                    .pow(exponent_modulus)),
                            std::false_type{})
                 : Fraction(Int(static_cast<BaseInt>(_denominator)
                                    .pow(exponent_modulus)),
                            Int(static_cast<BaseInt>(_numerator)
                                    .pow(exponent_modulus)),
                            std::false_type{});
    }
    return Fraction(Int(static_cast<BaseInt>(_numerator).pow(exponent)),
                    Int(static_cast<BaseInt>(_denominator).pow(exponent)),
                    std::false_type{});
  }

  py::object pow(const Fraction& exponent) const {
    return exponent._denominator.is_one()
               ? py::cast(pow(exponent._numerator))
               : ::pow(py::float_{double{*this}}, py::float_{double{exponent}});
  }

  int sign() const { return _numerator.sign(); }

  operator Int() const { return sign() < 0 ? ceil() : floor(); }

 private:
  Int _numerator, _denominator;

  template <bool NORMALIZE>
  Fraction(const Int& numerator, const Int& denominator,
           std::bool_constant<NORMALIZE>)
      : _numerator(numerator), _denominator(denominator) {
    if constexpr (NORMALIZE) {
      if (_denominator.sign() == 0) {
        PyErr_SetString(PyExc_ZeroDivisionError,
                        "Denominator should not be zero.");
        throw py::error_already_set();
      }
      if (_denominator.sign() < 0) {
        _numerator = -_numerator;
        _denominator = -_denominator;
      }
      Int gcd = _numerator.gcd(_denominator);
      _denominator = _denominator.floor_divide(gcd);
      _numerator = _numerator.floor_divide(gcd);
    }
  }
};

static Fraction operator+(const Int& self, const Fraction& other) {
  return Fraction(self * other.denominator() + other.numerator(),
                  other.denominator());
}

static Fraction operator-(const Int& self, const Fraction& other) {
  return Fraction(self * other.denominator() - other.numerator(),
                  other.denominator());
}

static Fraction operator%(const Int& self, const Fraction& other) {
  return Fraction((self * other.denominator()) % other.numerator(),
                  other.denominator());
}

static Fraction operator*(const Int& self, const Fraction& other) {
  const Int self_other_denominator_gcd = self.gcd(other.denominator());
  return Fraction(
      self.floor_divide(self_other_denominator_gcd) * other.numerator(),
      other.denominator().floor_divide(self_other_denominator_gcd));
}

static Fraction operator/(const Int& self, const Fraction& other) {
  const Int self_other_numerator_gcd = self.gcd(other.numerator());
  return Fraction(
      self.floor_divide(self_other_numerator_gcd) * other.denominator(),
      other.numerator().floor_divide(self_other_numerator_gcd));
}

Int floor_divide(const Int& self, const Fraction& other) {
  return (self * other.denominator()).floor_divide(other.numerator());
}

py::object pow(const Int& base, const Fraction& exponent) {
  return exponent.denominator().is_one()
             ? py::cast(Fraction(base).pow(exponent.numerator()))
             : pow(py::float_(double{base}), py::float_(double{exponent}));
}

static std::ostream& operator<<(std::ostream& stream, const Fraction& value) {
  return stream << C_STR(MODULE_NAME) "." FRACTION_NAME "(" << value.numerator()
                << ", " << value.denominator() << ")";
}

template <class Iterable>
IterableState iterable_to_state(const Iterable& self) {
  IterableState result;
  for (const auto& element : self) result.append(element);
  return result;
}

void fill_from_iterable(RawList& raw, const py::iterable& values) {
  auto position = py::iter(values);
  while (position != py::iterator::sentinel())
    raw.emplace_back(*(position++), true);
}

void fill_from_iterable(RawSet& raw, const py::iterable& values) {
  auto position = py::iter(values);
  while (position != py::iterator::sentinel()) raw.emplace(*(position++), true);
}

void fill_indices(const py::slice& slice, Size size, Index& start, Index& stop,
                  Index& step, Size& slice_length) {
  std::size_t raw_start, raw_stop, raw_step;
  if (!slice.compute(size, &raw_start, &raw_stop, &raw_step, &slice_length))
    throw py::error_already_set();
  start = static_cast<Index>(raw_start);
  stop = static_cast<Index>(raw_stop);
  step = static_cast<Index>(raw_step);
}

class ListIterator {
 public:
  static ListIterator from_state(IteratorState state) {
    if (state.size() != 3) throw std::runtime_error("Invalid state.");
    auto iterable_state = state[1].cast<IterableState>();
    auto raw = std::make_shared<RawList>();
    raw->reserve(iterable_state.size());
    for (auto& element : iterable_state)
      raw->push_back(py::reinterpret_borrow<Object>(element));
    return {state[0].cast<Size>(), std::move(raw), state[2].cast<bool>()};
  }

  static IteratorState to_state(const ListIterator& iterator) {
    return py::make_tuple(iterator._index, iterable_to_state(*iterator._raw),
                          iterator._running);
  }

  ListIterator(Size index, std::shared_ptr<RawList> raw, bool running = true)
      : _index(index), _raw(std::move(raw)), _running(running) {}

  Object next() {
    if (_running) {
      if (_index < _raw->size()) return (*_raw)[_index++];
      _running = false;
    }
    throw py::stop_iteration();
  }

 private:
  Size _index;
  std::shared_ptr<RawList> _raw;
  bool _running;
};

class ListReversedIterator {
 public:
  static ListReversedIterator from_state(IteratorState state) {
    if (state.size() != 3) throw std::runtime_error("Invalid state.");
    auto iterable_state = state[1].cast<IterableState>();
    auto raw = std::make_shared<RawList>();
    raw->reserve(iterable_state.size());
    for (auto& element : iterable_state)
      raw->push_back(py::reinterpret_borrow<Object>(element));
    return {state[0].cast<Size>(), std::move(raw), state[2].cast<bool>()};
  }

  static IteratorState to_state(const ListReversedIterator& iterator) {
    return py::make_tuple(iterator._index, iterable_to_state(*iterator._raw),
                          iterator._running);
  }

  ListReversedIterator(Size index, std::shared_ptr<RawList> raw,
                       bool running = true)
      : _index(index), _raw(std::move(raw)), _running(running) {}

  Object next() {
    if (_running) {
      if (_index && _index <= _raw->size()) return (*_raw)[--_index];
      _running = false;
    }
    throw py::stop_iteration();
  }

 private:
  Size _index;
  std::shared_ptr<RawList> _raw;
  bool _running;
};

void apply_permutation(RawList& raw, std::vector<Size>& indices) {
  using std::swap;
  for (Size index = 0; index < indices.size(); ++index) {
    auto cursor = index;
    while (index != indices[cursor]) {
      auto next = indices[cursor];
      swap(raw[cursor], raw[next]);
      indices[cursor] = cursor;
      cursor = next;
    }
    indices[cursor] = cursor;
  }
}

class List {
 public:
  static List from_state(IterableState state) {
    RawList raw;
    raw.reserve(state.size());
    for (auto& element : state)
      raw.push_back(py::reinterpret_borrow<Object>(element));
    return {raw};
  }

  static IterableState to_state(const List& list) {
    return iterable_to_state(*list._raw);
  }

  List(const RawList& raw) : _raw(std::make_shared<RawList>(raw)) {}

  List(py::iterable values) : _raw(std::make_shared<RawList>()) {
    fill_from_iterable(*_raw, values);
  }

  bool operator==(const List& other) const { return *_raw == *other._raw; }

  bool operator<(const List& other) const {
    return std::lexicographical_compare(_raw->begin(), _raw->end(),
                                        other._raw->begin(), other._raw->end());
  }

  bool operator<=(const List& other) const { return !(other < *this); }

  List operator+(const List& other) const {
    RawList raw{*_raw};
    raw.insert(raw.end(), other._raw->begin(), other._raw->end());
    return {raw};
  }

  operator bool() const { return !_raw->empty(); }

  void append(Object value) { _raw->push_back(value); }

  void clear() { _raw->clear(); }

  bool contains(Object value) const {
    return std::find(_raw->begin(), _raw->end(), value) != _raw->end();
  }

  List copy() const { return {*this}; }

  std::size_t count(Object value) const {
    return std::count(_raw->begin(), _raw->end(), value);
  }

  void delete_item(Index index) {
    Index size = _raw->size();
    Index normalized_index = index >= 0 ? index : index + size;
    if (normalized_index < 0 || normalized_index >= size)
      throw py::index_error(size ? (std::string("Index should be in range(") +
                                    std::to_string(-size) + ", " +
                                    std::to_string(size) + "), but found " +
                                    std::to_string(index) + ".")
                                 : std::string("Sequence is empty."));
    _raw->erase(_raw->begin() + normalized_index);
  }

  void delete_items(py::slice slice) {
    auto size = _raw->size();
    Index start, stop, step;
    Size slice_length;
    fill_indices(slice, size, start, stop, step, slice_length);
    if (step > 0 ? start >= stop : start <= stop) return;
    if (step == 1)
      _raw->erase(std::next(_raw->begin(), start),
                  std::next(_raw->begin(), stop));
    else if (step == -1)
      _raw->erase(std::next(_raw->begin(), stop + 1),
                  std::next(_raw->begin(), start + 1));
    else if (step > 0) {
      const auto& begin = _raw->begin();
      RawList raw{begin, std::next(begin, start)};
      raw.reserve(size - slice_length);
      for (; step < stop - start; start += step)
        raw.insert(raw.end(), std::next(begin, start + 1),
                   std::next(begin, start + step));
      raw.insert(raw.end(), std::next(begin, start + 1), _raw->end());
      _raw->assign(raw.begin(), raw.end());
    } else {
      start = size - start - 1;
      stop = size - stop - 1;
      step = -step;
      const auto& rbegin = _raw->rbegin();
      RawList raw{rbegin, std::next(rbegin, start)};
      raw.reserve(size - slice_length);
      for (; step < stop - start; start += step)
        raw.insert(raw.end(), std::next(rbegin, start + 1),
                   std::next(rbegin, start + step));
      raw.insert(raw.end(), std::next(rbegin, start + 1), _raw->rend());
      _raw->assign(raw.rbegin(), raw.rend());
    }
  }

  void extend(py::iterable iterable) { fill_from_iterable(*_raw, iterable); }

  Object get_item(Index index) const {
    Index size = _raw->size();
    Index normalized_index = index >= 0 ? index : index + size;
    if (normalized_index < 0 || normalized_index >= size)
      throw py::index_error(size ? (std::string("Index should be in range(" +
                                                std::to_string(-size) + ", ") +
                                    std::to_string(size) + "), but found " +
                                    std::to_string(index) + ".")
                                 : std::string("Sequence is empty."));
    return (*_raw)[normalized_index];
  }

  List get_items(py::slice slice) const {
    Index start, stop, step;
    Size slice_length;
    fill_indices(slice, _raw->size(), start, stop, step, slice_length);
    RawList raw;
    raw.reserve(slice_length);
    if (step < 0)
      for (; start > stop; start += step) raw.push_back((*_raw)[start]);
    else
      for (; start < stop; start += step) raw.push_back((*_raw)[start]);
    return List{raw};
  }

  Index index(Object value, Index start, Index stop) const {
    Index size = _raw->size();
    auto normalized_start =
        std::max(std::min(start >= 0 ? start : start + size, size),
                 static_cast<Index>(0));
    auto normalized_stop = std::max(
        std::min(stop >= 0 ? stop : stop + size, size), static_cast<Index>(0));
    for (Index index = normalized_start; index < normalized_stop; ++index)
      if ((*_raw)[index] == value) return index;
    throw py::value_error(object_to_repr(value) + " is not found in slice(" +
                          std::to_string(normalized_start) + ", " +
                          std::to_string(normalized_stop) + ").");
  }

  void insert(Index index, Object value) {
    Index size = _raw->size();
    std::size_t normalized_index =
        std::max(std::min(index >= 0 ? index : index + size, size),
                 static_cast<Index>(0));
    _raw->insert(_raw->begin() + normalized_index, value);
  }

  ListIterator iter() const { return {0, _raw}; }

  Object pop(Index index) {
    Index size = _raw->size();
    Index normalized_index = index >= 0 ? index : index + size;
    if (normalized_index < 0 || normalized_index >= size)
      throw py::index_error(size ? (std::string("Index should be in range(") +
                                    std::to_string(-size) + ", " +
                                    std::to_string(size) + "), but found " +
                                    std::to_string(index) + ".")
                                 : std::string("List is empty."));
    if (normalized_index == size - 1) {
      auto result = _raw->back();
      _raw->pop_back();
      return result;
    }
    auto result = (*_raw)[normalized_index];
    _raw->erase(std::next(_raw->begin(), normalized_index));
    return result;
  }

  void remove(Object value) {
    const auto& end = _raw->end();
    const auto& position = std::find(_raw->begin(), end, value);
    if (position == end)
      throw py::value_error(object_to_repr(value) + " is not found.");
    _raw->erase(position);
  }

  void reverse() { std::reverse(_raw->begin(), _raw->end()); }

  ListReversedIterator reversed() { return {_raw->size(), _raw}; }

  void set_item(Index index, Object value) {
    Index size = _raw->size();
    Index normalized_index = index >= 0 ? index : index + size;
    if (normalized_index < 0 || normalized_index >= size)
      throw py::index_error(size ? (std::string("Index should be in range(" +
                                                std::to_string(-size) + ", ") +
                                    std::to_string(size) + "), but found " +
                                    std::to_string(index) + ".")
                                 : std::string("Sequence is empty."));
    (*_raw)[normalized_index] = value;
  }

  void set_items(py::slice slice, py::iterable values) {
    auto size = _raw->size();
    Index start, stop, step;
    Size slice_length;
    fill_indices(slice, size, start, stop, step, slice_length);
    RawList raw;
    fill_from_iterable(raw, values);
    auto values_count = raw.size();
    if (step == 1) {
      auto new_size = size - slice_length + values_count;
      if (new_size > size) {
        _raw->resize(new_size, py::none{});
        const auto& last_replaced =
            std::next(_raw->begin(), std::max(start, stop) - 1);
        for (auto source = std::next(_raw->begin(), size - 1),
                  destination = std::next(_raw->begin(), new_size - 1);
             source != last_replaced; --source, --destination)
          std::iter_swap(source, destination);
      } else if (new_size < size) {
        const auto& old_end = _raw->end();
        for (auto source = std::next(_raw->begin(), stop),
                  destination = std::next(_raw->begin(), start + values_count);
             source != old_end; ++source, ++destination)
          std::iter_swap(source, destination);
        _raw->erase(std::next(_raw->begin(), new_size), old_end);
      }
      std::copy(raw.begin(), raw.end(), std::next(_raw->begin(), start));
      return;
    }
    if (slice_length != values_count)
      throw py::value_error(
          std::string("Attempt to assign iterable with capacity") +
          std::to_string(values_count) + " to slice with size " +
          std::to_string(slice_length) + ".");
    auto position = raw.begin();
    if (step < 0)
      for (; start > stop; start += step) (*_raw)[start] = *(position++);
    else
      for (; start < stop; start += step) (*_raw)[start] = *(position++);
  }

  std::size_t size() const { return _raw->size(); }

  void sort(Object key, bool reverse) {
    if (reverse) std::reverse(_raw->begin(), _raw->end());
    if (key.is_none())
      std::stable_sort(_raw->begin(), _raw->end());
    else {
      RawList& values = *_raw;
      const Size size = values.size();
      RawList keys;
      keys.reserve(size);
      for (const auto& value : values) keys.push_back(key(value));
      std::vector<Size> indices;
      indices.reserve(size);
      for (Size index = 0; index < size; ++index) indices.push_back(index);
      std::stable_sort(
          indices.begin(), indices.end(),
          [&keys](Size left, Size right) { return keys[left] < keys[right]; });
      apply_permutation(values, indices);
    }
    if (reverse) std::reverse(_raw->begin(), _raw->end());
  }

 private:
  std::shared_ptr<RawList> _raw;
};

static std::ostream& operator<<(std::ostream& stream, const List& list) {
  stream << C_STR(MODULE_NAME) "." LIST_NAME "([";
  auto object = py::cast(list);
  if (Py_ReprEnter(object.ptr()) == 0) {
    if (list) {
      stream << list.get_item(0);
      for (std::size_t index = 1; index < list.size(); ++index)
        stream << ", " << list.get_item(index);
    }
    Py_ReprLeave(object.ptr());
  } else {
    stream << "...";
  }
  return stream << "])";
}

class SetIterator {
 public:
  using Position = RawSet::const_iterator;

  static IterableState to_state(const SetIterator& set) {
    return iterable_to_state(*set._raw);
  }

  SetIterator(Position position, std::shared_ptr<RawSet> raw, Token token)
      : _position(position),
        _raw(std::move(raw)),
        _token(std::move(token)),
        _running(true) {}

  Object next() {
    if (_running) {
      if (_token.expired())
        throw std::runtime_error("Set modified during iteration.");
      if (_position != _raw->cend()) return *_position++;
      _running = false;
    }
    throw py::stop_iteration();
  }

 private:
  Position _position;
  std::shared_ptr<RawSet> _raw;
  Token _token;
  bool _running;
};

static bool is_raw_set_disjoint_with(const RawSet& smaller_set,
                                     const RawSet& bigger_set) {
  const auto& bigger_set_end = bigger_set.cend();
  for (const auto& element : smaller_set)
    if (bigger_set.find(element) != bigger_set_end) return false;
  return true;
}

static bool is_raw_set_subset_of(const RawSet& left, const RawSet& right) {
  if (left.size() > right.size()) return false;
  const auto& right_end = right.cend();
  for (const auto& element : left)
    if (right.find(element) == right_end) return false;
  return true;
}

static void raw_sets_in_place_difference(RawSet& left, const RawSet& right) {
  const auto& right_end = right.cend();
  for (auto position = left.cbegin(); position != left.cend();)
    if (right.find(*position) != right_end)
      position = left.erase(position);
    else
      ++position;
}

static void raw_sets_in_place_intersection(RawSet& left, const RawSet& right) {
  const auto& right_end = right.cend();
  for (auto position = left.cbegin(); position != left.cend();)
    if (right.find(*position) == right_end)
      position = left.erase(position);
    else
      ++position;
}

static void raw_sets_in_place_symmetric_difference(RawSet& left,
                                                   const RawSet& right) {
  for (const auto& element : right) {
    const auto& position = left.find(element);
    if (position == left.cend())
      left.insert(element);
    else
      left.erase(position);
  }
}

static RawSet raw_sets_intersection(const RawSet& smaller_set,
                                    const RawSet& bigger_set) {
  RawSet result;
  const auto& bigger_set_end = bigger_set.cend();
  for (const auto& element : smaller_set)
    if (bigger_set.find(element) != bigger_set_end) result.insert(element);
  return result;
}

static RawSet raw_sets_symmetric_difference(const RawSet& smaller_set,
                                            const RawSet& bigger_set) {
  RawSet result{bigger_set};
  for (const auto& element : smaller_set) {
    const auto& position = result.find(element);
    if (position == result.cend())
      result.insert(element);
    else
      result.erase(position);
  }
  return result;
}

static RawSet raw_sets_union(const RawSet& smaller_set,
                             const RawSet& bigger_set) {
  RawSet result{bigger_set};
  result.insert(smaller_set.cbegin(), smaller_set.cend());
  return result;
}

class Set {
 public:
  static Set from_state(IterableState state) {
    RawSet raw;
    raw.reserve(state.size());
    for (auto& element : state)
      raw.insert(py::reinterpret_borrow<Object>(element));
    return {raw};
  }

  static IterableState to_state(const Set& set) {
    return iterable_to_state(*set._raw);
  }

  Set(const RawSet& raw) : _raw(std::make_shared<RawSet>(raw)), _tokenizer() {}

  Set(const std::shared_ptr<RawSet>& raw) : _raw(raw), _tokenizer() {}

  Set(py::iterable values) : _raw(std::make_shared<RawSet>()), _tokenizer() {
    fill_from_iterable(*_raw, values);
  }

  Set operator&(const Set& other) const {
    return {_raw->size() < other._raw->size()
                ? raw_sets_intersection(*_raw, *other._raw)
                : raw_sets_intersection(*other._raw, *_raw)};
  }

  Set& operator&=(const Set& other) {
    auto& raw = *_raw;
    auto size_before = raw.size();
    raw_sets_in_place_intersection(raw, *other._raw);
    if (raw.size() != size_before) _tokenizer.reset();
    return *this;
  }

  Set operator-(const Set& other) const {
    RawSet result;
    const auto& other_raw = *other._raw;
    auto end = other_raw.cend();
    for (const auto& element : *_raw)
      if (other_raw.find(element) == end) result.insert(element);
    return {result};
  }

  Set& operator-=(const Set& other) {
    auto& raw = *_raw;
    auto size_before = raw.size();
    raw_sets_in_place_difference(raw, *other._raw);
    if (raw.size() != size_before) _tokenizer.reset();
    return *this;
  }

  bool operator<(const Set& other) const {
    const auto& raw = *_raw;
    const auto& other_raw = *other._raw;
    if (raw.size() >= other_raw.size()) return false;
    const auto& other_end = other_raw.cend();
    for (const auto& element : raw)
      if (other_raw.find(element) == other_end) return false;
    return true;
  }

  bool operator<=(const Set& other) const {
    return is_raw_set_subset_of(*_raw, *other._raw);
  }

  bool operator==(const Set& other) const { return *_raw == *other._raw; }

  Set operator^(const Set& other) const {
    return {_raw->size() < other._raw->size()
                ? raw_sets_symmetric_difference(*_raw, *other._raw)
                : raw_sets_symmetric_difference(*other._raw, *_raw)};
  }

  Set& operator^=(const Set& other) {
    if (!other._raw->empty()) _tokenizer.reset();
    raw_sets_in_place_symmetric_difference(*_raw, *other._raw);
    return *this;
  }

  Set operator|(const Set& other) const {
    return {_raw->size() < other._raw->size()
                ? raw_sets_union(*_raw, *other._raw)
                : raw_sets_union(*other._raw, *_raw)};
  }

  Set& operator|=(const Set& other) {
    auto size_before = _raw->size();
    _raw->insert(other._raw->cbegin(), other._raw->cend());
    if (_raw->size() != size_before) _tokenizer.reset();
    return *this;
  }

  operator bool() const { return !_raw->empty(); }

  void add(const Object& value) {
    if (_raw->insert(value).second) _tokenizer.reset();
  }

  void clear() {
    _tokenizer.reset();
    _raw->clear();
  }

  bool contains(const Object& value) const {
    return _raw->find(value) != _raw->cend();
  }

  Set copy() const { return {_raw}; }

  Set difference(py::args others) const {
    RawSet raw{*_raw};
    for (const auto& other : others) {
      RawSet values;
      fill_from_iterable(values, other.cast<py::iterable>());
      raw_sets_in_place_difference(raw, values);
    }
    return {raw};
  }

  void difference_update(py::args others) {
    auto& raw = *_raw;
    auto size_before = raw.size();
    for (const auto& other : others) {
      RawSet values;
      fill_from_iterable(values, other.cast<py::iterable>());
      raw_sets_in_place_difference(raw, values);
    }
    if (raw.size() != size_before) _tokenizer.reset();
  }

  void discard(const Object& value) {
    if (_raw->erase(value)) _tokenizer.reset();
  }

  Set intersection(py::args others) const {
    RawSet raw{*_raw};
    for (const auto& other : others) {
      RawSet values;
      fill_from_iterable(values, other.cast<py::iterable>());
      raw_sets_in_place_intersection(raw, values);
    }
    return {raw};
  }

  void intersection_update(py::args others) {
    auto& raw = *_raw;
    auto size_before = raw.size();
    for (const auto& other : others) {
      RawSet values;
      fill_from_iterable(values, other.cast<py::iterable>());
      raw_sets_in_place_intersection(raw, values);
    }
    if (raw.size() != size_before) _tokenizer.reset();
  }

  bool isdisjoint(py::iterable other) {
    RawSet values;
    fill_from_iterable(values, other);
    const auto& raw = *_raw;
    return raw.size() < values.size() ? is_raw_set_disjoint_with(raw, values)
                                      : is_raw_set_disjoint_with(values, raw);
  }

  bool issubset(py::iterable other) {
    RawSet values;
    fill_from_iterable(values, other);
    return is_raw_set_subset_of(*_raw, values);
  }

  bool issuperset(py::iterable other) {
    RawSet values;
    fill_from_iterable(values, other);
    return is_raw_set_subset_of(values, *_raw);
  }

  SetIterator iter() const {
    return {_raw->cbegin(), _raw, _tokenizer.create()};
  }

  Object pop() {
    if (_raw->empty()) throw py::key_error("Pop from empty set.");
    _tokenizer.reset();
    auto position = _raw->begin();
    Object result = *position;
    _raw->erase(position);
    return result;
  }

  void remove(const Object& value) {
    if (_raw->erase(value))
      _tokenizer.reset();
    else
      throw py::key_error(object_to_repr(value));
  }

  std::size_t size() const { return _raw->size(); }

  Set symmetric_difference(py::iterable other) const {
    RawSet values;
    fill_from_iterable(values, other);
    RawSet raw{*_raw};
    raw_sets_in_place_symmetric_difference(raw, values);
    return {raw};
  }

  void symmetric_difference_update(py::iterable other) {
    RawSet values;
    fill_from_iterable(values, other);
    if (!values.empty()) _tokenizer.reset();
    raw_sets_in_place_symmetric_difference(*_raw, values);
  }

  Set union_(py::args others) const {
    RawSet raw{*_raw};
    for (const auto& other : others) {
      RawList values;
      fill_from_iterable(values, other.cast<py::iterable>());
      raw.insert(values.begin(), values.end());
    }
    return {raw};
  }

  void update(py::args others) {
    auto& raw = *_raw;
    auto size_before = raw.size();
    for (const auto& other : others) {
      RawList values;
      fill_from_iterable(values, other.cast<py::iterable>());
      raw.insert(values.begin(), values.end());
    }
    if (raw.size() != size_before) _tokenizer.reset();
  }

 private:
  std::shared_ptr<RawSet> _raw;
  Tokenizer _tokenizer;
};

static std::ostream& operator<<(std::ostream& stream, const Set& set) {
  stream << C_STR(MODULE_NAME) "." SET_NAME "([";
  auto object = py::cast(set);
  if (Py_ReprEnter(object.ptr()) == 0) {
    if (set) {
      auto iterator = set.iter();
      stream << iterator.next();
      for (std::size_t index = 1; index < set.size(); ++index)
        stream << ", " << iterator.next();
    }
    Py_ReprLeave(object.ptr());
  } else {
    stream << "...";
  }
  return stream << "])";
}

PYBIND11_MODULE(MODULE_NAME, m) {
  m.doc() =
      R"pbdoc(Alternative implementation of python builtins based on C++ `std` library.)pbdoc";
  m.attr("__version__") = C_STR(VERSION_INFO);

  m.def("gcd", &Int::gcd);

  py::class_<Int> PyInt(m, INT_NAME);
  PyInt.def(py::init<>())
      .def(py::init<const py::str&, std::size_t>(), py::arg("string"),
           py::arg("base") = 10)
      .def(py::init<const py::int_&>(), py::arg("value"))
      .def(py::self + py::self)
      .def(py::self == py::self)
      .def(~py::self)
      .def(py::self <= py::self)
      .def(py::self < py::self)
      .def(py::self % py::self)
      .def(py::self * py::self)
      .def(-py::self)
      .def(+py::self)
      .def(py::self - py::self)
      .def(py::self / py::self)
      .def(py::pickle([](const Int& self) { return py::int_(self); },
                      [](const py::int_& state) { return Int(state); }))
      .def("__abs__", &Int::abs)
      .def("__bool__", &Int::operator bool)
      .def("__ceil__", &identity<const Int&>)
      .def("__copy__", [](const Int& self) -> const Int& { return self; })
      .def("__divmod__", &Int::divmod, py::is_operator{})
      .def("__deepcopy__",
           [](const Int& self, const py::dict&) -> Int { return self; })
      .def("__float__", &Int::operator double)
      .def("__floor__", &identity<const Int&>)
      .def("__floordiv__", &Int::floor_divide, py::is_operator{})
      .def("__hash__", &Int::hash)
      .def("__int__", &Int::operator py::int_)
      .def("__pow__", &Int::pow, py::arg("exponent"),
           py::arg("modulus") = nullptr, py::is_operator{})
      .def("__repr__", &to_repr<Int>)
      .def("__str__", &Int::repr<10>)
      .def("__trunc__", &identity<const Int&>);

  py::class_<Fraction> PyFraction(m, FRACTION_NAME);
  PyFraction.def(py::init<>())
      .def(py::init<const Int&, const Int&>(), py::arg("numerator"),
           py::arg("denominator") = Int(1))
      .def(py::self + py::self)
      .def(py::self + Int{})
      .def(py::self == py::self)
      .def(py::self <= py::self)
      .def(py::self < py::self)
      .def(py::self % py::self)
      .def(py::self % Int{})
      .def(py::self * py::self)
      .def(py::self * Int{})
      .def(-py::self)
      .def(+py::self)
      .def(Int{} + py::self)
      .def(Int{} % py::self)
      .def(Int{} * py::self)
      .def(Int{} - py::self)
      .def(Int{} / py::self)
      .def(py::self - py::self)
      .def(py::self - Int{})
      .def(py::self / py::self)
      .def(py::self / Int{})
      .def(py::pickle(
          [](const Fraction& value) {
            return py::make_tuple(value.numerator(), value.denominator());
          },
          [](const py::tuple& state) {
            if (state.size() != 2) throw std::runtime_error("Invalid state.");
            return Fraction(state[0].cast<Int>(), state[1].cast<Int>());
          }))
      .def("__bool__", &Fraction::operator bool)
      .def("__ceil__", &Fraction::ceil)
      .def("__divmod__", &Fraction::divmod, py::is_operator{})
      .def("__float__", &Fraction::operator double)
      .def("__floor__", &Fraction::floor)
      .def("__floordiv__",
           py::overload_cast<const Fraction&>(&Fraction::floor_divide,
                                              py::const_),
           py::is_operator{})
      .def("__floordiv__",
           py::overload_cast<const Int&>(&Fraction::floor_divide, py::const_),
           py::is_operator{})
      .def("__hash__", &Fraction::hash)
      .def("__int__", [](const Fraction& self) { return py::int_(Int(self)); })
      .def("__pow__", py::overload_cast<const Int&>(&Fraction::pow, py::const_),
           py::arg("exponent"), py::is_operator{})
      .def("__pow__",
           py::overload_cast<const Fraction&>(&Fraction::pow, py::const_),
           py::arg("exponent"), py::is_operator{})
      .def("__repr__", &to_repr<Fraction>)
      .def(
          "__rfloordiv__",
          [](const Fraction& divisor, const Int& dividend) {
            return floor_divide(dividend, divisor);
          },
          py::is_operator{})
      .def(
          "__rpow__",
          [](const Fraction& exponent, const Int& base) {
            return pow(base, exponent);
          },
          py::is_operator{})
      .def("__str__",
           [](const Fraction& self) {
             return self.denominator().is_one()
                        ? self.numerator().repr()
                        : self.numerator().repr() + "/" +
                              self.denominator().repr();
           })
      .def("__trunc__", &Fraction::operator Int)
      .def_property_readonly("denominator", &Fraction::denominator)
      .def_property_readonly("numerator", &Fraction::numerator);

  py::class_<List> PyList(m, LIST_NAME);
  PyList.def(py::init<py::iterable>(), py::arg("values"))
      .def(py::self == py::self)
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self + py::self)
      .def(py::pickle(&List::to_state, &List::from_state))
      .def("__bool__", &List::operator bool)
      .def("__contains__", &List::contains, py::arg("value"))
      .def("__delitem__", &List::delete_item, py::arg("index"))
      .def("__delitem__", &List::delete_items, py::arg("slice"))
      .def("__getitem__", &List::get_item, py::arg("index"))
      .def("__getitem__", &List::get_items, py::arg("slice"))
      .def(
          "__iadd__",
          [](List& self, py::iterable iterable) {
            self.extend(iterable);
            return self;
          },
          py::arg("values"), py::is_operator{})
      .def("__iter__", &List::iter)
      .def("__len__", &List::size)
      .def("__repr__", &to_repr<List>)
      .def("__reversed__", &List::reversed)
      .def("__setitem__", &List::set_item, py::arg("index"), py::arg("value"))
      .def("__setitem__", &List::set_items, py::arg("slice"), py::arg("values"))
      .def("append", &List::append, py::arg("value"))
      .def("clear", &List::clear)
      .def("copy", &List::copy)
      .def("count", &List::count, py::arg("value"))
      .def("extend", &List::extend, py::arg("values"))
      .def("index", &List::index, py::arg("value"), py::arg("start") = 0,
           py::arg("stop") = std::numeric_limits<Index>::max())
      .def("insert", &List::insert, py::arg("index"), py::arg("value"))
      .def("pop", &List::pop, py::arg("index") = -1)
      .def("remove", &List::remove, py::arg("value"))
      .def("reverse", &List::reverse)
      .def("sort", &List::sort, py::arg("key") = py::none(),
           py::arg("reverse") = false);

  py::module collections_abc = py::module::import("collections.abc");
  collections_abc.attr("MutableSequence").attr("register")(PyList);

  py::class_<ListIterator>(m, LIST_ITERATOR_NAME)
      .def(py::pickle(&ListIterator::to_state, &ListIterator::from_state))
      .def("__iter__", &identity<const ListIterator&>)
      .def("__next__", &ListIterator::next);

  py::class_<ListReversedIterator>(m, LIST_REVERSED_ITERATOR_NAME)
      .def(py::pickle(&ListReversedIterator::to_state,
                      &ListReversedIterator::from_state))
      .def("__iter__", &identity<const ListReversedIterator&>)
      .def("__next__", &ListReversedIterator::next);

  py::class_<Set> PySet(m, SET_NAME);
  PySet.def(py::init<py::iterable>(), py::arg("values"))
      .def(py::self & py::self, py::arg("other"))
      .def(py::self &= py::self, py::arg("other"))
      .def(py::self - py::self, py::arg("other"))
      .def(py::self -= py::self, py::arg("other"))
      .def(py::self < py::self, py::arg("other"))
      .def(py::self <= py::self, py::arg("other"))
      .def(py::self == py::self, py::arg("other"))
      .def(py::self ^ py::self, py::arg("other"))
      .def(py::self ^= py::self, py::arg("other"))
      .def(py::self | py::self, py::arg("other"))
      .def(py::self |= py::self, py::arg("other"))
      .def(py::pickle(&Set::to_state, &Set::from_state))
      .def("__bool__", &Set::operator bool)
      .def("__contains__", &Set::contains, py::arg("value"))
      .def("__iter__", &Set::iter)
      .def("__len__", &Set::size)
      .def("__repr__", &to_repr<Set>)
      .def("add", &Set::add, py::arg("value"))
      .def("clear", &Set::clear)
      .def("copy", &Set::copy)
      .def("difference", &Set::difference)
      .def("difference_update", &Set::difference_update)
      .def("discard", &Set::discard, py::arg("value"))
      .def("intersection", &Set::intersection)
      .def("intersection_update", &Set::intersection_update)
      .def("isdisjoint", &Set::isdisjoint, py::arg("other"))
      .def("issubset", &Set::issubset, py::arg("other"))
      .def("issuperset", &Set::issuperset, py::arg("other"))
      .def("pop", &Set::pop)
      .def("remove", &Set::remove, py::arg("value"))
      .def("symmetric_difference", &Set::symmetric_difference, py::arg("other"))
      .def("symmetric_difference_update", &Set::symmetric_difference_update,
           py::arg("other"))
      .def("union", &Set::union_)
      .def("update", &Set::update);

  collections_abc.attr("MutableSet").attr("register")(PySet);

  py::class_<SetIterator>(m, SET_ITERATOR_NAME)
      .def(
          "__reduce__",
          [PyList = PyList, iter = py::module::import("builtins").attr("iter")](
              const SetIterator& iterator) {
            return py::make_tuple(iter, py::make_tuple(PyList(py::iter(
                                            SetIterator::to_state(iterator)))));
          })
      .def("__iter__", &identity<const SetIterator&>)
      .def("__next__", &SetIterator::next);
}
