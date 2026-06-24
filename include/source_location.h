#ifndef SOURCE_LOCATION_H
#define SOURCE_LOCATION_H

#include <cstdint>
#include <string_view>

struct SourceLocation {
  uint64_t line{1}, column{0};
  std::string_view filename = "";
};

#endif  // SOURCE_LOCATION_H
