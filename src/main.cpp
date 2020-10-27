#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace py = pybind11;

#define MODULE_NAME cppbuiltins
#define C_STR_HELPER(a) #a
#define C_STR(a) C_STR_HELPER(a)
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
std::string to_repr(const Type& value) {
  std::ostringstream stream;
  stream.precision(std::numeric_limits<double>::digits10 + 2);
  stream << value;
  return {stream.str()};
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

  bool operator<=(const List& other) const {
    return *this < other || *this == other;
  }

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
      for (const auto value : values) keys.push_back(key(value));
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

  bool operator<(const Set& other) const {
    if (_raw->size() >= other._raw->size()) return false;
    auto other_end = other._raw->cend();
    for (const auto& element : *_raw)
      if (other._raw->find(element) == other_end) return false;
    return true;
  }

  bool operator<=(const Set& other) const {
    if (_raw->size() > other._raw->size()) return false;
    auto other_end = other._raw->cend();
    for (const auto& element : *_raw)
      if (other._raw->find(element) == other_end) return false;
    return true;
  }

  bool operator==(const Set& other) const { return *_raw == *other._raw; }

  Set& operator|=(const Set& other) {
    auto size_before = _raw->size();
    _raw->insert(other._raw->cbegin(), other._raw->cend());
    if (_raw->size() != size_before) _tokenizer.reset();
    return *this;
  }

  Set operator|(const Set& other) const {
    if (_raw->size() < other._raw->size()) {
      RawSet raw{*other._raw};
      raw.insert(_raw->cbegin(), _raw->cend());
      return {raw};
    } else {
      RawSet raw{*_raw};
      raw.insert(other._raw->cbegin(), other._raw->cend());
      return {raw};
    }
  }

  operator bool() const { return !_raw->empty(); }

  void add(const Object& value) {
    if (_raw->insert(value).second) _tokenizer.reset();
  }

  Set copy() const { return {_raw}; }

  void clear() {
    _tokenizer.reset();
    _raw->clear();
  }

  bool contains(const Object& value) const {
    return _raw->find(value) != _raw->cend();
  }

  void discard(const Object& value) {
    if (_raw->erase(value)) _tokenizer.reset();
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

  void update(py::iterable iterable) {
    RawList values;
    fill_from_iterable(values, iterable);
    auto size_before = _raw->size();
    _raw->insert(values.begin(), values.end());
    if (_raw->size() != size_before) _tokenizer.reset();
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
  m.attr("__version__") = VERSION_INFO;

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
      .def(py::self < py::self)
      .def(py::self <= py::self)
      .def(py::self == py::self)
      .def(py::self | py::self)
      .def(py::self |= py::self)
      .def(py::pickle(&Set::to_state, &Set::from_state))
      .def("__bool__", &Set::operator bool)
      .def("__contains__", &Set::contains)
      .def("__iter__", &Set::iter)
      .def("__len__", &Set::size)
      .def("__repr__", &to_repr<Set>)
      .def("add", &Set::add)
      .def("clear", &Set::clear)
      .def("copy", &Set::copy)
      .def("discard", &Set::discard)
      .def("pop", &Set::pop)
      .def("remove", &Set::remove)
      .def("update", &Set::update);

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
