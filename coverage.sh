#!/bin/sh
set -e

# Run tests and output raw profile directly to build folder
LLVM_PROFILE_FILE="build/code-%p.profraw" ./build/tests/run_tests

# Merge coverage data
llvm-profdata merge -sparse build/*.profraw -o build/coverage.profdata

# Generate HTML report (filtered to src/ and include/)
llvm-cov show ./build/tests/run_tests -instr-profile=build/coverage.profdata -format=html -output-dir=build/coverage_report src/ include/

# Show summary in terminal (filtered to src/ and include/)
llvm-cov report ./build/tests/run_tests -instr-profile=build/coverage.profdata src/ include/

