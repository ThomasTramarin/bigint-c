#pragma once

#include "bigint.h"
#include <stddef.h>
#include <stdint.h>

struct bint {
  uint32_t *limbs; // array of limbs (32-bit integer)
  size_t cap;      // allocated capacity (in limbs)
  size_t size;     // current size (in limbs)
  int8_t sign;     // -1, 0, +1
};

void *bint__malloc(size_t size);
void *bint__realloc(void *p, size_t size);

void bint__reserve_limbs(bint *b, size_t new_cap);
void bint__normalize(bint *b);

int bint__cmp_abs(const bint *a, const bint *b);

void bint__add_abs(bint *dst, const bint *a, const bint *b);
void bint__sub_abs(bint *dst, const bint *a, const bint *b);