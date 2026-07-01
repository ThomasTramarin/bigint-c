# bint - C BigInt Library

## Index
- [Overview](#overview)  
- [Implementation Design](#implementation-design)  
- [API Design](#api-design)  
- [Public API Reference](#public-api-reference)  

## Overview
bint is a C library for arbitrary-precision signed integers.

This project is primarily educational and aims to explore low-level implementation of big integers, memory management, and arithmetic algorithms in C.

## Implementation Design

### Number Representation
The library implements integer using a base $2^{32}$ (word-based) representation.

Each integer is stored as an array of 32-bit unsigned integers (called limbs), where each limb represents a digit in base $2^{32}$.

The value of a bigint is defined as:
$value = sign \cdot \sum_{i=0}^{size-1} l_i \cdot 2^{32i}$

Where:
- $l_i$ is the $i$-th limb (a 32-bit unsigned integer)
- $l_0$ is the least significant limb
- $size$ is the number of significant limbs
- $sign \in \{-1,0,1\}$ is the sign of the number

The limb array always stores the magnitude (absolute value) of the number, while the sign is stored separately.

This means that:
- The limbs encode $|value|$.
- The sign determines whether the magnitude is interpreted as positive, negative or zero.

### Memory Layout

Limbs are stored in little-endian order:
- index `0` corresponds to the least significant 32 bits
- index `n - 1` corresponds to the most significant 32 bits


### Canonical Form
A bigint is in canonical form if:

- The representation always contains at least one limb:  
  - $size \ge 1$ for all valid bigints  
- No leading zero limbs exist (the most significant limb is non-zero unless the number is zero)
- Zero is uniquely represented as:
  - $size = 1$  
  - $l_0 = 0$  
  - $sign = 0$  
- Sign normalization rule:  
  - if $value = 0$, then $sign = 0$  
  - otherwise $sign \in$ {+1, -1}  

## API Design

### Overview 

- **Opaque Type**: the internal structure of `bint` is hidden from the user.
- **Explicit Ownership**: all `bint*` returned bu the API are heap-allocated and must be freed using `bint_free`.
- **Reusable Instances**: a `bint` instance is mutable and can be reassigned multiple times using assignment functions.
- **Dynamic Capacity Management**: assignment functions automatically manage internal memory.
- **Error Model**: the library is error-free except for allocation failures (OOM), which are considered fatal by default. The user can register an handler using `bint_set_oom_handler` to catch these errors.


### Lifecycle 
A `bint` must be:
1. Created via `bint_new`.
2. Updated via assignment functions.
3. Freed via `bint_free`.

A `bint` can be reused across multiple assignment (each assignment overwrites the previous value).

### Error Management
By default, on allocation failure the following function will be called:

```c
static void default_oom(void) {
  fprintf(stderr, "bint: out of memory\n");
  abort();
}
```

The user is free to rewrite this handler function (e.g. to perform clean-up operations) by registering it using `bint_set_oom_handler`.

## Public API Reference

### `bint_new`
```c
bint *bint_new(void);
```

Allocates and returns a new heap-allocated `bint`.

- The returned object is initialized to zero in canonical form and is valid to use.

- The memory must be freed by the caller using `bint_free`.


### `bint_free`
```c
void bint_free(bint *b);
```

Releases all memory associated with a `bint`.
- Safe to pass `NULL`.
- After calling this function, the pointer becomes invalid.
- The caller should set the pointer to `NULL` after freeing to avoid dangling references.

### `bint_clone`
```c
bint *bint_clone(const bint *src);
```

Creates a new heap-allocated `bint` that is an exact copy of `src`.
- Allocates a new `bint` on the heap.
- Copies the full value of `src`.
- The returned object is independed from the source.
- The caller is responsible for freeubt it using `bint_free`.

### `bint_set_oom_handler`
```c
typedef void (*bint_oom_fn)(void);
void bint_set_oom_handler(bint_oom_fn fn);
```
Registers a `bint_oom_fn` function that is invoked whenever the library fails to allocate memory.
- Calling this function overrides the default behaviour (program termination).
- Passing `NULL` restores the default handler.

> The handler must not return. Returning from the handler results in undefined behavior. 

### `bint_assign`
```c
void bint_assign(bint *dst, const bint *src)
```

Copies the value of `src` to `dst`.
- Overwrites the current value stored in `dst`.
- Automatically adjust capacity if needed.
- Performs a deep copy of the internal limb array.
- After assignment, `dst` is independent from `src`.


### `bint_assign_i64`
```c
void bint_assign_i64(bint *b, int64_t value);
```

Assigns a signed 64-bit integer value to a `bint`.

- Overwrites the current value stored in `b`.
- Automatically resizes internal storage if required.
- May reuse previously allocated memory if capacity is sufficient.

### `bint_add`
```c
void bint_add(bint *dst, const bint *a, const bint *b);
```

Computes the sum: `dst = a + b`.
- Supports signed integers.
- Handles all sign combinations (`+/+`, `+/-`, `-/+`, `-/-`)
- Safe for aliasing (`dst == a` or `dst == b`)

### `bint_add`
```c
void bint_sub(bint *dst, const bint *a, const bint *b);
```
Computes the difference: `dst = a - b`.
- Supports signed integers.
- Internally implemented with `bint_add` (equivalent to `dst = a + (-b)`).
- Safe for aliasing (`dst == a` or `dst == b`).

### `bint_abs`
```c
void bint_abs(bint *dst, const bint *src);
```

Computes the absolute value of `src` and stores the result in `dst`.
- If `src` is positive or zero, `dst` becomes a copy of `src`.
- If `src` is negative, the sign is flipped to positive.
- Does not modify `src`.
- Safe for `dst == src` (in-place operation supported).