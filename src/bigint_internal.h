#pragma once

#include <stddef.h>
#include <stdint.h>

struct bint {
  uint32_t *limbs; // array of limbs (32-bit integer)
  size_t cap;      // allocated capacity (in limbs)
  size_t size;     // current size (in limbs)
  int8_t sign;     // -1, 0, +1
};