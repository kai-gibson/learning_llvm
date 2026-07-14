#ifndef FRONTEND_H
#define FRONTEND_H

#include <string>

void compile(const std::string& filename, bool output_llvm = false,
             bool output_tokens = false);

#endif  // FRONTEND_H
