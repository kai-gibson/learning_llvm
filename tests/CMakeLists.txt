include(FetchContent)

# Prevent GoogleTest from overriding our compiler/linker options if using MSVC
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(run_tests 
  lexer_test.cxx
)

# test executable links against .o from parent
target_link_libraries(run_tests PRIVATE 
  learning_llvm_lib 
  gtest_main
)

if(ENABLE_COVERAGE)
    target_compile_options(run_tests PRIVATE ${COVERAGE_FLAGS})
    target_link_options(run_tests PRIVATE ${COVERAGE_FLAGS})
endif()

include(GoogleTest)
gtest_discover_tests(run_tests)
