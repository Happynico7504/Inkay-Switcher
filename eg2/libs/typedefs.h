#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef unsigned int uint;

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int_least8_t    sl8;
typedef int_least16_t   sl16;
typedef int_least32_t   sl32;
typedef int_least64_t   sl64;

typedef uint_least8_t   ul8;
typedef uint_least16_t  ul16;
typedef uint_least32_t  ul32;
typedef uint_least64_t  ul64;

typedef int_fast8_t     sf8;
typedef int_fast16_t    sf16;
typedef int_fast32_t    sf32;
typedef int_fast64_t    sf64;

typedef uint_fast8_t    uf8;
typedef uint_fast16_t   uf16;
typedef uint_fast32_t   uf32;
typedef uint_fast64_t   uf64;

typedef float     f32;
typedef double    f64;

typedef uintptr_t uptr;
typedef intptr_t  sptr;

typedef size_t    usize;

#ifdef __cplusplus
}
#endif