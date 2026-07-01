
#include "test.h"
#include <stdint.h>
#include <stdio.h>

int test_passed;
int test_current;

void test_addition(void) {
  bint *a = bint_new();
  bint *b = bint_new();
  bint *c = bint_new();

  // --- ZERO CASES ---

  // 0 + 0
  bint_assign_i64(a, 0);
  bint_assign_i64(b, 0);
  bint_add(c, a, b);
  bint_assert(c->sign == 0);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 0);

  // 0 + positive
  bint_assign_i64(a, 0);
  bint_assign_i64(b, 0x40);
  bint_add(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 0x40);

  // positive + 0
  bint_assign_i64(a, 0x40);
  bint_assign_i64(b, 0);
  bint_add(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 0x40);

  // 0 + negative
  bint_assign_i64(a, 0);
  bint_assign_i64(b, -50);
  bint_add(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 50);

  // negative + 0
  bint_assign_i64(a, -50);
  bint_assign_i64(b, 0);
  bint_add(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 50);

  // --- SAME SIGN ---

  // positive + positive (without carry)
  bint_assign_i64(a, 10);
  bint_assign_i64(b, 20);
  bint_add(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 30);

  // positive + positive (with carry)
  bint_assign_i64(a, 0xFFFFFFFF);
  bint_assign_i64(b, 1);
  bint_add(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 2);
  bint_assert(c->limbs[0] == 0);
  bint_assert(c->limbs[1] == 1);

  // negative + negative (without carry)
  bint_assign_i64(a, -10);
  bint_assign_i64(b, -20);
  bint_add(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 30);

  // negative + negative (with carry)
  bint_assign_i64(a, -(int64_t)0xFFFFFFFF);
  bint_assign_i64(b, -1);
  bint_add(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 2);
  bint_assert(c->limbs[0] == 0);
  bint_assert(c->limbs[1] == 1);

  // --- DIFFERENT SIGN ---

  // positive + negative (positive result)
  bint_assign_i64(a, 100);
  bint_assign_i64(b, -30);
  bint_add(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 70);

  // positive + negative (negative result)
  bint_assign_i64(a, 30);
  bint_assign_i64(b, -100);
  bint_add(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 70);

  // negative + positive (positive result)
  bint_assign_i64(a, -30);
  bint_assign_i64(b, 100);
  bint_add(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 70);

  // negative + positive (negative result)
  bint_assign_i64(a, -100);
  bint_assign_i64(b, 30);
  bint_add(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 70);

  // positive + negative (result = 0)
  bint_assign_i64(a, 50);
  bint_assign_i64(b, -50);
  bint_add(c, a, b);
  bint_assert(c->sign == 0);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 0);

  // a = a + b
  bint_assign_i64(a, 10);
  bint_assign_i64(b, 14);
  bint_add(a, a, b);
  bint_assert(a->sign == 1);
  bint_assert(a->size == 1);
  bint_assert(a->limbs[0] == 24);

  // a = a + a
  bint_assign_i64(a, 14);
  bint_add(a, a, a);
  bint_assert(a->sign == 1);
  bint_assert(a->size == 1);
  bint_assert(a->limbs[0] == 28);

  bint_free(a);
  bint_free(b);
  bint_free(c);

  a = NULL;
  b = NULL;
  c = NULL;
}

void test_subtraction(void) {
  bint *a = bint_new();
  bint *b = bint_new();
  bint *c = bint_new();

  // --- ZERO CASES ---

  // 0 - 0
  bint_assign_i64(a, 0);
  bint_assign_i64(b, 0);
  bint_sub(c, a, b);
  bint_assert(c->sign == 0);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 0);

  // 0 - positive = negative
  bint_assign_i64(a, 0);
  bint_assign_i64(b, 50);
  bint_sub(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 50);

  // 0 - negative = positive
  bint_assign_i64(a, 0);
  bint_assign_i64(b, -50);
  bint_sub(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 50);

  // positive - 0
  bint_assign_i64(a, 40);
  bint_assign_i64(b, 0);
  bint_sub(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 40);

  // negative - 0
  bint_assign_i64(a, -40);
  bint_assign_i64(b, 0);
  bint_sub(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 40);

  // --- SAME SIGN ---

  // positive - positive (a > b)
  bint_assign_i64(a, 100);
  bint_assign_i64(b, 30);
  bint_sub(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 70);

  // positive - positive (a < b)
  bint_assign_i64(a, 30);
  bint_assign_i64(b, 100);
  bint_sub(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 70);

  // positive - positive (equal)
  bint_assign_i64(a, 50);
  bint_assign_i64(b, 50);
  bint_sub(c, a, b);
  bint_assert(c->sign == 0);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 0);

  // negative - negative (a < b in value)
  bint_assign_i64(a, -30);
  bint_assign_i64(b, -100);
  bint_sub(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 70);

  // negative - negative (a > b in value)
  bint_assign_i64(a, -100);
  bint_assign_i64(b, -30);
  bint_sub(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 70);

  // --- DIFFERENT SIGN ---

  // positive - negative = addition
  bint_assign_i64(a, 100);
  bint_assign_i64(b, -30);
  bint_sub(c, a, b);
  bint_assert(c->sign == 1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 130);

  // negative - positive = negative addition
  bint_assign_i64(a, -100);
  bint_assign_i64(b, 30);
  bint_sub(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 130);

  // positive - positive large result sign flip
  bint_assign_i64(a, 30);
  bint_assign_i64(b, 100);
  bint_sub(c, a, b);
  bint_assert(c->sign == -1);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 70);

  // result = 0
  bint_assign_i64(a, 50);
  bint_assign_i64(b, 50);
  bint_sub(c, a, b);
  bint_assert(c->sign == 0);
  bint_assert(c->size == 1);
  bint_assert(c->limbs[0] == 0);

  bint_free(a);
  bint_free(b);
  bint_free(c);
}

int main() {

  // initialization
  bint *b;

  b = bint_new();

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

  // smallest 64 bit int (INT64_MIN + 1)
  bint_assign_i64(b, INT64_MIN + 1);
  bint_assert(b->size == 2);
  bint_assert(b->sign == -1);
  bint_assert(b->limbs[0] == 0xFFFFFFFF);
  bint_assert(b->limbs[1] == 0x7FFFFFFF);

  bint_free(b);
  b = NULL;

  test_addition();
  test_subtraction();

  printf("[END] Test (total: %d, passed: %d)\n", test_current, test_passed);

  return 0;
}