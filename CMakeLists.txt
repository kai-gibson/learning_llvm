cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# silence alignment errros on coverage linking
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.7" CACHE STRING "Minimum OS X version" FORCE)


project(LearningLLVM VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

option(ENABLE_COVERAGE "Enable LLVM coverage" OFF)

separate_arguments(LLVM_DEFINITIONS_LIST UNIX_COMMAND "${LLVM_DEFINITIONS}")

llvm_map_components_to_libnames(llvm_libs
    core executionengine mcjit native x86codegen
)
list(REMOVE_DUPLICATES llvm_libs)

set(COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/CLI11/include
)

set(COMMON_COMPILE_OPTIONS
    -Wall -Wextra -Wpedantic
    -Wno-deprecated-declarations -Wno-unused-parameter
)

# Seperate library for testability

add_library(learning_llvm_lib OBJECT
    src/frontend.cxx
    src/lexer.cxx
    src/file_io.cxx
    src/parser.cxx
    src/ast.cxx
    src/type_check_visitor.cxx
    src/codegen/code_generation_visitor.cxx
    src/codegen/operation_map.cxx
)

target_include_directories(learning_llvm_lib
    PRIVATE ${COMMON_INCLUDE_DIRS}
    SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS}
)
target_compile_definitions(learning_llvm_lib PRIVATE
    ${LLVM_DEFINITIONS_LIST}
    _LIBCPP_ENABLE_CXX17_REMOVED_FEATURES
)
target_compile_options(learning_llvm_lib PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_libraries(learning_llvm_lib PUBLIC ${llvm_libs})

# Final executable
add_executable(learning_llvm src/main.cxx)
target_include_directories(learning_llvm PRIVATE ${COMMON_INCLUDE_DIRS})
target_compile_options(learning_llvm PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_libraries(learning_llvm PRIVATE learning_llvm_lib)

if(ENABLE_COVERAGE)
    set(COVERAGE_FLAGS "-fprofile-instr-generate;-fcoverage-mapping")
    target_compile_options(learning_llvm_lib PRIVATE ${COVERAGE_FLAGS})
    target_link_options(learning_llvm_lib PRIVATE ${COVERAGE_FLAGS})
    target_link_options(learning_llvm PRIVATE ${COVERAGE_FLAGS})
endif()

enable_testing()

add_subdirectory(tests)
