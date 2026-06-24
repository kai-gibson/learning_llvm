#ifndef SOURCE_LOCATION_H
#define SOURCE_LOCATION_H

#include <cstdint>

struct SourceLocation {
  uint64_t line{1}, column{1};
  const char* filename = "";
};

#endif  // SOURCE_LOCATION_H
