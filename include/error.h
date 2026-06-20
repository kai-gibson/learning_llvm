#ifndef ERROR_H
#define ERROR_H
#include <stdexcept>

struct Error : std::runtime_error {
  Error(const std::exception& e) _NOEXCEPT : std::runtime_error(e.what()) {}
  Error(const std::string& s) _NOEXCEPT : std::runtime_error(s) {}
  Error(const char* s) _NOEXCEPT : std::runtime_error(s) {}
};
#endif  // ERROR_H
