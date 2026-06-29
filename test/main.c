
#include "test.h"
#include <stdint.h>
#include <stdio.h>

int test_passed;
int test_current;

int main() {

  // initialization
  bint *b = bint_new();

  bint_assert(b != NULL);
  bint_assert(b->cap == 1);
  bint_assert(b->size == 1);
  bint_assert(b->limbs != NULL);
  bint_assert(b->limbs[0] == 0);

  // assignments
  bint_assign_i64(b, 0);
  bint_assert(b->size == 1);
  bint_assert(b->sign == 0);

  // 32 bit positive integer
  bint_assign_i64(b, 123456789LL);
  bint_assert(b->size == 1);
  bint_assert(b->sign == 1);
  bint_assert(b->limbs[0] == 123456789LL);

  // 32 bit negative integer
  bint_assign_i64(b, -987654321LL);
  bint_assert(b->size == 1);
  bint_assert(b->sign == -1);
  bint_assert(b->limbs[0] == 987654321LL);

  // biggest 64 bit int (INT64_MAX)
  bint_assign_i64(b, INT64_MAX);
  bint_assert(b->size == 2);
  bint_assert(b->sign == 1);
  bint_assert(b->limbs[0] == 0xFFFFFFFF);
  bint_assert(b->limbs[1] == 0x7FFFFFFF);

  // lowest 64 bit int (INT64_MIN + 1)
  bint_assign_i64(b, INT64_MIN + 1);
  bint_assert(b->size == 2);
  bint_assert(b->sign == -1);
  bint_assert(b->limbs[0] == 0xFFFFFFFF);
  bint_assert(b->limbs[1] == 0x7FFFFFFF);

  bint_free(b);

  printf("[END] Test (total: %d, passed: %d)\n", test_current, test_passed);

  return 0;
}