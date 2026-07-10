include(FetchContent)

# Prevent GoogleTest from overriding our compiler/linker options if using MSVC
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(run_tests
  lexer_test.cxx
  utils_test.cxx
  parser_test.cxx
)

target_include_directories(run_tests PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
  ../include/
  include/codegen/
)

# test executable links against .o from parent
target_link_libraries(run_tests PRIVATE
  learning_llvm_lib
  project_settings
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(run_tests)
