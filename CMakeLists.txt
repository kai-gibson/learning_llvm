cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

project(LearningLLVM VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress deprecation warnings from LLVM
add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-unused-parameter")

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

# Include LLVM headers and definitions
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST UNIX_COMMAND "${LLVM_DEFINITIONS}")
add_definitions(${LLVM_DEFINITIONS_LIST})

# Find required LLVM components (simplified - only what we actually need)
llvm_map_components_to_libnames(llvm_libs
    core
    executionengine
    mcjit
    native
    x86codegen
)

# Remove duplicates
list(REMOVE_DUPLICATES llvm_libs)

# Create the executable
add_executable(learning_llvm 
  src/main.cxx 
  src/lexer.cxx
  src/file_io.cxx
  src/parser.cxx
  src/ast.cxx
  src/type_check_visitor.cxx
  src/codegen/code_generation_visitor.cxx
  src/codegen/operation_map.cxx
)

# Link LLVM libraries
target_link_libraries(learning_llvm ${llvm_libs})

target_include_directories(learning_llvm PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external/CLI11/include
)
target_include_directories(learning_llvm SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})

# Compiler flags
target_compile_options(learning_llvm PRIVATE -Wall -Wextra -Wpedantic)
