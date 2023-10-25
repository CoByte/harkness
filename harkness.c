// Copyright 2023 Owen Wheary

#include "./harkness.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#ifndef TEST
#ifndef TEST_ALL
#define DONT_RUN_TESTS
#endif /* ifndef TEST_ALL */
#endif /* ifndef TEST */

#ifndef TEST
#define TEST ""
#endif /* ifndef TEST */

#ifndef TEST_ALL
#define TEST_ALL 0
#endif /* ifndef TEST_ALL */

// Used to stringify a macro expansion.
#define QUOTE(...) #__VA_ARGS__
#define STR(...) QUOTE(__VA_ARGS__)

// Stringifies TEST
#define TEST_STR STR(TEST)

/**
 * Parses test_input into an array of tests.
 *
 * Params:
 *  test_input - The input buffer. Is destructively modified. Output strings
 *    will refer into it.
 *  test_output - Out param. Heap-allocated array of char *s.
 *
 * Returns: The size of test_output.
 */
int parse_tests(char *test_input, char ***test_output) {
  // Counts the number of commas in test_input.
  int comma_count = 0;
  for (char *cur_c = test_input; *cur_c; ++cur_c) {
    if (*cur_c == ',') {
      ++comma_count;
    }
  }

  // Stores the tests to run in `tests`, and the number of tests in `test_num`
  char **tests = malloc((comma_count + 1) * sizeof(char *));
  int test_num = 0;
  char *saveptr, *curr;
  curr = strtok_r(test_input, ",", &saveptr);
  while (curr) {
    tests[test_num++] = curr;
    curr = strtok_r(NULL, ",", &saveptr);
  }

  *test_output = tests;
  return test_num;
}

/**
 * Checks for the presence of a string in an array of strings.
 */
int str_in_array(char *str, char **array, int size) {
  for (int i = 0; i < size; ++i) {
    if (strcmp(str, array[i]) == 0) {
      return 1;
    }
  }

  return 0;
}

// If you try to compile with testing on but no tests, the program will explode
// because there won't be anything in the "test_array" section. To fix this, I
// took the lazy way out, and added a fake test to ensure that the section is
// always initialized, and then ignored it during running.
#ifndef DONT_RUN_TESTS
h_test_nn(__NOT_A_TEST);
void __NOT_A_TEST(h_context *ctx) {}
#endif /* ifndef DONT_RUN_TESTS */

void run_tests() {
#ifndef DONT_RUN_TESTS
  // stores TEST_STR in a mutable variable.
  char test_input[] = TEST_STR;
  char **tests;
  int tests_size = parse_tests(test_input, &tests);

  h_context ctx = {
      .passed = 0,
      .failed = 0,
  };

  //  This for loop operates on, you guessed it, magic! Specifically, the
  //  very poorly documented `__start_##SECTION` and `__stop_##SECTION` magic
  //  variables (source: https://stackoverflow.com/a/16552711). Basically,
  //  GCC automagically initializes any variable with those names (where
  //  ##SECTION is the name of a custom section) to the start and end of the
  //  section. You can then reference it, and pretend the whole section is an
  //  array!
  //
  // Citation: https://stackoverflow.com/a/37082249
  for (t_test_func *entry = ({
         extern t_test_func __start_test_array;
         &__start_test_array;
       });
       entry != ({
         extern t_test_func __stop_test_array;
         &__stop_test_array;
       });
       ++entry) {
    // ignore the fake test.
    if (entry->callback == __NOT_A_TEST) {
      continue;
    }
    if (TEST_ALL || str_in_array(entry->name, tests, tests_size)) {
      fprintf(stderr, "════ TESTING `%s` ════\n", entry->name);
      entry->callback(&ctx);
      fprintf(stderr, "\n");
    }
  }

  fprintf(stderr, "════ TEST SUMMARY ════\n");
  fprintf(stderr, "Tests " PASSED ": %d\n", ctx.passed);

  if (ctx.failed > 0) {
    fprintf(stderr, "Tests " FAILED ": %d\n", ctx.failed);
  } else {
    fprintf(stderr, "All tests " PASSED "!\n");
  }
  fprintf(stderr, "══════════════════════\n");

  free(tests);

  // Exits early so only test code runs when testing.
  exit(0);
#endif
}

// cleans up macros
#undef TEST_STR
#undef QUOTE
#undef STR
