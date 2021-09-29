#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

namespace cppbuiltins {
class ZeroDivisionError : public std::exception {
 public:
  const char* what() const noexcept override {
    return "Division by zero is undefined.";
  }
};
}  // namespace cppbuiltins

#endif
