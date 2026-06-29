#!/bin/sh

cmake -Bdebug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm@19/bin/clang++ -DCMAKE_C_COMPILER=/usr/local/opt/llvm@19/bin/clang
