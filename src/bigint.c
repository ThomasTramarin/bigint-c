#include "bigint.h"
#include "bigint_internal.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// --- OOM Handler ---
static void default_oom(void) {
  fprintf(stderr, "bint: out of memory\n");
  abort();
}

bint_oom_fn oom_handler = default_oom;
void bint_set_oom_handler(bint_oom_fn fn) {
  oom_handler = fn ? fn : default_oom; // if NULL, set the default handler
}

// --- Heap Memory Management ---
static void *bint_malloc(size_t size) {
  void *p = malloc(size);
  if (!p)
    oom_handler();
  return p;
}

static void *bint_realloc(void *p, size_t size) {
  void *q = realloc(p, size);
  if (!q)
    oom_handler();

  return q;
}

static void bint_reserve_limbs(bint *b, size_t new_cap) {

  if (new_cap <= b->cap)
    return;

  b->cap = new_cap;
  b->limbs = bint_realloc(b->limbs, sizeof(uint32_t) * new_cap);
}

// --- Lifecycle ---
bint *bint_new(void) {
  bint *b = bint_malloc(sizeof(bint));

  bint_reserve_limbs(b, 1);
  b->sign = 0;
  b->size = 1;
  b->limbs[0] = 0;

  return b;
}

void bint_free(bint *b) {

  if (!b)
    return;

  if (b->limbs)
    free(b->limbs);

  free(b);
}

// --- Assignments ---
void bint_assign_i64(bint *b, int64_t value) {
  if (!b)
    return;

  b->sign = 0;
  b->size = 0;

  bint_reserve_limbs(b, 2);

  // zero in canonical form
  if (value == 0) {
    b->size = 1;
    b->sign = 0;
    b->limbs[0] = 0;

    return;
  }

  uint64_t abs = 0;

  // if value is negative
  if (value < 0) {
    b->sign = -1;
    abs = (uint64_t)(-(value + 1)) + 1;
  } else if (value > 0) {
    // if value is positive
    b->sign = 1;
    abs = (uint64_t)value;
  }

  int i = 0;
  while (abs != 0) {
    b->limbs[i] = (uint32_t)(abs & 0xFFFFFFFF); // get last 32 bits
    abs >>= 32;
    i++;
  }

  b->size = i;

  return;
}