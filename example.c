// Copyright 2023 Owen Wheary

#include "./harkness.h"
#include <stdio.h>

h_test_nn(early_test);
void early_test(h_context *ctx) { h_assert(1); }

// this is a function we want to test.
int foo(int a, int b) {
  if (a < 5) {
    return b;
  } else {
    return a;
  }
}

h_test_nn(passing_test);
void passing_test(h_context *ctx) { h_assert(1); }

h_test_nn(failing_test);
void failing_test(h_context *ctx) { h_assert(0); }

h_test_nn(test_assert_int);
void test_assert_int(h_context *ctx) {
  h_assert_int(foo(6, 10), ==, 6);
  h_assert_int(foo(6, 10), ==, 7);
}

h_test_nn(test_assert);
void test_assert(h_context *ctx) {
  h_assert(1 == 1);
  h_assert(1 == 2)
}

h_test_nn(test_non_null);
void test_non_null(h_context *ctx) {
  int foo = 12;
  int *t1 = &foo;
  int *t2 = NULL;

  h_assert_non_null(t1);
  h_assert_non_null(t2);
}

h_test_nn(test_str_eq);
void test_str_eq(h_context *ctx) {
  char *str1 = "hello";
  char *str2 = "world";

  h_assert_str_eq(str1, str1);
  h_assert_str_eq(str1, str2);
}

int main() {
  run_tests();

  printf("This shouldn't run when we're testing!!\n");

  return 0;
}
