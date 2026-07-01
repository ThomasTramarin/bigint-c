#include "bigint_internal.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- OOM Handler ---
static void default_oom(void) {
  fprintf(stderr, "bint: out of memory\n");
  abort();
}

static bint_oom_fn oom_handler = default_oom;
void bint_set_oom_handler(bint_oom_fn fn) {
  oom_handler = fn ? fn : default_oom; // if NULL, set the default handler
}

// --- Heap Memory Management ---
void *bint__malloc(size_t size) {
  void *p = malloc(size);
  if (!p)
    oom_handler();
  return p;
}

void *bint__realloc(void *p, size_t size) {
  void *q = realloc(p, size);
  if (!q)
    oom_handler();

  return q;
}

void bint__reserve_limbs(bint *b, size_t new_cap) {

  if (new_cap <= b->cap)
    return;

  b->limbs = bint__realloc(b->limbs, sizeof(uint32_t) * new_cap);
  b->cap = new_cap;
}

// --- Utils ---
void bint__normalize(bint *b) {
  // remove leading zeros
  while (b->size > 1 && b->limbs[b->size - 1] == 0)
    b->size--;

  // normalize sign
  if (b->size == 1 && b->limbs[0] == 0)
    b->sign = 0;
}

// --- Lifecycle ---
bint *bint_new(void) {
  bint *b = bint__malloc(sizeof(bint));

  memset(b, 0, sizeof(*b));

  bint__reserve_limbs(b, 1);
  b->size = 1;
  b->limbs[0] = 0;

  return b;
}

void bint_free(bint *b) {
  // NULL safety
  if (!b)
    return;

  if (b->limbs)
    free(b->limbs);

  free(b);
}

bint *bint_clone(const bint *src) {
  bint *b = bint_new();

  bint_assign(b, src);

  return b;
}

// --- Assignments ---

void bint_assign(bint *dst, const bint *src) {

  if (dst == src)
    return;

  bint__reserve_limbs(dst, src->cap);

  dst->size = src->size;
  dst->sign = src->sign;

  memcpy(dst->limbs, src->limbs, src->size * sizeof(uint32_t));

  bint__normalize(dst);
}

void bint_assign_i64(bint *b, int64_t value) {
  if (!b)
    return;

  b->sign = 0;
  b->size = 0;

  bint__reserve_limbs(b, 2);

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

// --- Compare ---
int bint__cmp_abs(const bint *a, const bint *b) {
  if (a->size > b->size)
    return 1;

  if (b->size > a->size)
    return -1;

  // equal size
  for (int i = (int)a->size - 1; i >= 0; i--) {
    uint32_t ai = a->limbs[i];
    uint32_t bi = b->limbs[i];

    if (ai == bi)
      continue;

    if (ai > bi)
      return 1;

    if (bi > ai)
      return -1;
  }

  return 0;
}

// --- Operations ---

// dst = |a| + |b|. The sign is set by the caller
void bint__add_abs(bint *dst, const bint *a, const bint *b) {
  size_t max = (a->size > b->size ? a->size : b->size);
  bint__reserve_limbs(dst, max + 1); // reserve one more byte (carry)

  uint64_t carry = 0;
  uint64_t sum = 0;

  size_t i;
  for (i = 0; i < max; i++) {
    uint32_t ai = (i < a->size) ? a->limbs[i] : 0;
    uint32_t bi = (i < b->size) ? b->limbs[i] : 0;

    sum = (uint64_t)ai + bi + carry;

    dst->limbs[i] = (uint32_t)sum;

    carry = sum >> 32;
  }

  if (carry) {
    dst->limbs[i++] = (uint32_t)carry;
  }

  dst->size = i;

  bint__normalize(dst);
}

// dst = |a| - |b|. The sign is set by the caller (|a| must be greater than |b|)
void bint__sub_abs(bint *dst, const bint *a, const bint *b) {

  bint__reserve_limbs(dst, a->size);

  uint64_t borrow = 0;
  uint64_t sub = 0;

  size_t i;
  for (i = 0; i < a->size; i++) {

    uint32_t ai = a->limbs[i];
    uint32_t bi = (i < b->size) ? b->limbs[i] : 0;

    sub = ai - bi - borrow;

    if (ai < bi + borrow) {
      borrow = 1;
      sub += (uint64_t)1 << 32;
    } else {
      borrow = 0;
    }

    dst->limbs[i] = (uint32_t)sub;
  }

  dst->size = a->size;

  bint__normalize(dst);
}

void bint_add(bint *dst, const bint *a, const bint *b) {

  assert(dst && a && b);

  bint *a_cpy = NULL;
  bint *b_cpy = NULL;

  if (dst == a) {
    a_cpy = bint_clone(a);
    a = a_cpy;
  }
  if (dst == b) {
    b_cpy = bint_clone(b);
    b = b_cpy;
  }
  // after this line, a and b are not dst

  if (a->sign == 0) {
    bint_assign(dst, b);
    goto end;
  }

  if (b->sign == 0) {
    bint_assign(dst, a);
    goto end;
  }

  if (a->sign == b->sign) {
    bint__add_abs(dst, a, b);
    dst->sign = a->sign;
  } else {
    int cmp = bint__cmp_abs(a, b);

    if (cmp == 0) {
      dst->sign = 0;
      bint__sub_abs(dst, a, b);
    } else if (cmp == 1) {
      dst->sign = a->sign;
      bint__sub_abs(dst, a, b);
    } else if (cmp == -1) {
      dst->sign = b->sign;
      bint__sub_abs(dst, b, a);
    }
  }

end:
  bint_free(a_cpy);
  bint_free(b_cpy);
}

void bint_sub(bint *dst, const bint *a, const bint *b) {
  assert(dst && a && b);

  bint *b_cpy = NULL;
  if (b->sign != 0) {
    b_cpy = bint_clone(b);
    b_cpy->sign = -b_cpy->sign;
    b = b_cpy;
  }

  bint_add(dst, a, b);

  bint_free(b_cpy);
}

void bint_abs(bint *dst, const bint *src) {
  if (dst != src)
    bint_assign(dst, src);

  if (dst->sign == -1)
    dst->sign = 1;
}