#pragma once

// Copyright 2023 Owen Wheary

#include "./colors.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Disables nice colors when NO_COLOR is set
// Useful for outputing to contexts where ANSI escape sequences don't work (like
// plaintext files).
#ifndef NO_COLOR
#define PASSED ANSI_COLOR_GREEN "PASSED" ANSI_COLOR_RESET
#define FAILED ANSI_COLOR_RED "FAILED" ANSI_COLOR_RESET
#define expr_literal(expr) ANSI_COLOR_MAGENTA "`" expr "`" ANSI_COLOR_RESET
#define expr_value(expr) ANSI_COLOR_YELLOW "`" expr "`" ANSI_COLOR_RESET
#define str_format() ANSI_COLOR_CYAN "\"%s\"" ANSI_COLOR_RESET
#else
#define PASSED "PASSED"
#define FAILED "FAILED"
#define expr_literal(expr) "`" expr "`"
#define expr_value(expr) "`" expr "`"
#define str_format() "\"%s\""
#endif /* COLOR_MODE */

#ifndef TEST
#ifndef TEST_ALL
#define DONT_RUN_TESTS
#endif /* ifndef TEST_ALL */
#endif /* ifndef TEST */

// All the code up until `#define tassert` was heavily inspired by a very
// helpful Stack Overflow post, and adapted for my own use.
//
// Citation: https://stackoverflow.com/a/37082249

// Context for storing test metadata.
typedef struct h_context {
  int passed;
  int failed;
} h_context;

// Test functions should take a pointer to a text context !!CALLED `ctx`!! and
// return void.
typedef void (*test_func_callback)(h_context *ctx);

// The test function to callback, and the name of the function it tests (for
// more granular testing).
typedef struct s_test_func {
  test_func_callback callback;
  char *name;
} t_test_func;

// Test expands to a single variable definition. It is very cursed. I will give
// my best effort to explain what it is doing, bit by bit.
//
// static t_test_func s_##func_cb - The variable declaration. Macro
//   concatination is used to ensure that the variable name is unique in its
//   file.
// __attribute((used, section("test_array"))) - This one is a doozy. The `used`
//   bit prevents the compiler from optimizing out the variable, even though it
//   appears to be unused. The `section("test_array")` bit causes the variable
//   to be stored in a special section of the final ELF file. This is the magic
//   that makes the whole think work. By putting only variables of type
//   t_test_func in the section, it can be treated like an array using some
//   other macro magic in `run_tests`.
//
// The final bit just assigns a struct containing the callback and the name to
// the new variable.
/**
 * Registers a function to test.
 *
 * This macro *must* be called in the top level of the file, or bad things will
 * happen. It does forward declare its target function, so it can be placed
 * before the function is defined.
 *
 * params:
 * tname - The name of the test. Is typically the name of the function being
 *   tested, *not* the name of the test function itself.
 * func_cb - The name of the test function. The test function must be of type
 * `test_func_callback`.
 */
#ifndef DONT_RUN_TESTS
#define h_test(tname, func_cb)                                                 \
  void func_cb(h_context *ctx);                                                \
  static t_test_func s_##func_cb                                               \
      __attribute((used, section("test_array"))) = {                           \
          .callback = func_cb,                                                 \
          .name = #tname,                                                      \
  }
#else
#define h_test(tname, func_bc)
#endif /* DONT_RUN_TESTS */

// A wrapper around h_test when `tname` and `func_cb` are the same.
#define h_test_nn(tname) h_test(tname, tname)

// Used for printing the first part of a test.
#define h_test_header() fprintf(stderr, "%s:%d:\t ", __FILE__, __LINE__);

/**
 * Tests an expression. Designed for use in test functions.
 *
 * params:
 * expr - The expression to test. It is evaluated exactly once. The test passes
 *   if it evaluates to a truthy value, and fails if it evaluates to a falsy
 *   value.
 */
#define h_assert(expr)                                                         \
  do {                                                                         \
    h_test_header();                                                           \
    fprintf(stderr, expr_literal(#expr) " ... ");                              \
    if (expr) {                                                                \
      fprintf(stderr, PASSED ".\n");                                           \
      ctx->passed++;                                                           \
    } else {                                                                   \
      fprintf(stderr, FAILED ".\n");                                           \
      ctx->failed++;                                                           \
    }                                                                          \
  } while (0);

// Creates h_assert_TYPEs
#define h_assert_generic_compare(T, format, lhs, op, rhs)                      \
  do {                                                                         \
    h_test_header();                                                           \
    fprintf(stderr, expr_literal(#lhs " " #op " " #rhs) " ... ");              \
    T out_lhs = lhs;                                                           \
    T out_rhs = rhs;                                                           \
    if (out_lhs op out_rhs) {                                                  \
      fprintf(stderr, PASSED ".\n");                                           \
      ctx->passed++;                                                           \
    } else {                                                                   \
      fprintf(stderr,                                                          \
              FAILED ": " expr_literal(#lhs) " = " expr_value(format) ".\n",   \
              out_lhs);                                                        \
      ctx->failed++;                                                           \
    }                                                                          \
  } while (0);

/**
 * macro `h_assert_TYPE`
 *
 * Tests a binary operation between TYPE. Designed for use in test
 * functions. Provides better test output than `h_assert`.
 *
 * params:
 * lhs - The expression to test.
 * op - The operator to use. Should be one of C's binary operators.
 * rhs - The value to compare the test to. Should be a TYPE literal for better
 *   error messages.
 */
#define h_assert_int(lhs, op, rhs)                                             \
  h_assert_generic_compare(int, "%d", lhs, op, rhs)

#define h_assert_char(lhs, op, rhs)                                            \
  h_assert_generic_compare(char, "%c", lhs, op, rhs)

#define h_assert_float(lhs, op, rhs)                                           \
  h_assert_generic_compare(float, "%f", lhs, op, rhs)

/**
 * Tests a pointer to ensure it is non-null. Designed for use in test
 * functions. Gives better test output than h_assert.
 *
 * params:
 * ptr - the pointer to test.
 */
#define h_assert_non_null(ptr)                                                 \
  do {                                                                         \
    h_test_header();                                                           \
    fprintf(stderr,                                                            \
            expr_literal(#ptr) " (" expr_value(                                \
                "0x%" PRIxPTR) ") is non-null ... ",                           \
            (uintptr_t)ptr);                                                   \
    if (ptr) {                                                                 \
      fprintf(stderr, PASSED ".\n");                                           \
      ctx->passed++;                                                           \
    } else {                                                                   \
      fprintf(stderr, FAILED ".\n");                                           \
      ctx->failed++;                                                           \
    }                                                                          \
  } while (0);

/**
 * Tests a pointer to ensure it is null. Designed for use in test
 * functions. Gives better test output than h_assert.
 *
 * params:
 * ptr - the pointer to test.
 */
#define h_assert_null(ptr)                                                     \
  do {                                                                         \
    h_test_header();                                                           \
    fprintf(                                                                   \
        stderr,                                                                \
        expr_literal(#ptr) " (" expr_value("0x%" PRIxPTR) ") is null ... ",    \
        (uintptr_t)ptr);                                                       \
    if (!ptr) {                                                                \
      fprintf(stderr, PASSED ".\n");                                           \
      ctx->passed++;                                                           \
    } else {                                                                   \
      fprintf(stderr, FAILED ".\n");                                           \
      ctx->failed++;                                                           \
    }                                                                          \
  } while (0);

#define h_assert_str_eq(lhs, rhs)                                              \
  do {                                                                         \
    h_test_header();                                                           \
    fprintf(stderr, expr_literal(#lhs) " == " expr_literal(#rhs) " ... ");     \
    char *str_l = lhs;                                                         \
    char *str_r = rhs;                                                         \
    if (strcmp(str_l, str_r) == 0) {                                           \
      fprintf(stderr, PASSED ".\n");                                           \
      ctx->passed++;                                                           \
    } else {                                                                   \
      fprintf(stderr, FAILED ": " str_format() " != " str_format() "\n",       \
              str_l, str_r);                                                   \
      ctx->failed++;                                                           \
    }                                                                          \
  } while (0);

/**
 * Runs tests registered with `test`, then exits the program. Should be placed
 * at the top of main.
 *
 * This function is enabled with macro definitions that can be set with the `gcc
 * -D` flag. If any tests are enabled, run_tests will be defined. Otherwise, it
 * will be an empty function, that will allow normal execution.
 *
 *  Flags:
 *  -D TEST=name,of,tests - Runs the tests with the given names.
 *  -D TEST_ALL - Runs all tests regardless of what TEST is set to.
 *  -D COLOR_MODE - Activates pretty colors using ANSI escape sequences. Makes
 *     printed tests look nice, but less readable in text editors.
 */
void run_tests();
