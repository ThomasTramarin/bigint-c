#pragma once
#include "../src/bigint_internal.h"
#include <stdio.h>

extern int test_current;
extern int test_passed;

#define bint_assert(expr)                                                      \
  do {                                                                         \
    test_current++;                                                            \
    if (expr) {                                                                \
      test_passed++;                                                           \
      printf("[PASSED] Test #%d\n", test_current);                             \
    } else {                                                                   \
      printf("[FAILED] Test #%d (file: %s, line: %d)\n", test_current,         \
             __FILE__, __LINE__);                                              \
    }                                                                          \
  } while (0)
