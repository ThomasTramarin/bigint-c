#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct bint bint;

// --- Error Handling ---
typedef void (*bint_oom_fn)(void);
void bint_set_oom_handler(bint_oom_fn fn);

// --- Lifecycle ---
bint *bint_new(void);
bint *bint_clone(const bint *src);
void bint_free(bint *b);

// --- Assignments ---
void bint_assign(bint *dst, const bint *src);
void bint_assign_i64(bint *b, int64_t value);

// --- Compare ---
int bint_cmp_abs(const bint *a, const bint *b);

// --- Operations ---
void bint_add(bint *dst, const bint *a, const bint *b);
void bint_sub(bint *dst, const bint *a, const bint *b);
