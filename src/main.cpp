#include <pybind11/pybind11.h>

namespace py = pybind11;

#define MODULE_NAME cppbuiltins
#define C_STR_HELPER(a) #a
#define C_STR(a) C_STR_HELPER(a)
#ifndef VERSION_INFO
#define VERSION_INFO "dev"
#endif

PYBIND11_MODULE(MODULE_NAME, m) {
  m.doc() = R"pbdoc(Alternative implementation of python builtins based on C++ `std` library.)pbdoc";
  m.attr("__version__") = VERSION_INFO;
}
