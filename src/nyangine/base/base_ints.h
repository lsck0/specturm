#pragma once

#include <stdint.h>

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef uint8_t   b8;
typedef uint16_t  b16;
typedef uint32_t  b32;
typedef uint64_t  b64;
typedef void      u0;
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef uintptr_t uptr;
typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;
typedef intptr_t  sptr;
typedef _Float16  f16;
typedef float     f32;
typedef double    f64;

static_assert(sizeof(b8) == 1);
static_assert(sizeof(b16) == 2);
static_assert(sizeof(b32) == 4);
static_assert(sizeof(b64) == 8);
static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(s8) == 1);
static_assert(sizeof(s16) == 2);
static_assert(sizeof(s32) == 4);
static_assert(sizeof(s64) == 8);
static_assert(sizeof(f16) == 2);
static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * LIMITS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define U8_MIN  (0U)
#define U8_MAX  (255U)
#define U16_MIN (0U)
#define U16_MAX (65535U)
#define U32_MIN (0UL)
#define U32_MAX (4294967295UL)
#define U64_MIN (0ULL)
#define U64_MAX (18446744073709551615ULL)

#define S8_MIN  (-128)
#define S8_MAX  (127)
#define S16_MIN (-32768)
#define S16_MAX (32767)
#define S32_MIN (-2147483648L)
#define S32_MAX (2147483647L)
#define S64_MIN (-9223372036854775808LL)
#define S64_MAX (9223372036854775807LL)

#define F16_MIN (-65504.0f)
#define F16_MAX (65504.0f)
#define F32_MIN (-__FLT_MAX__)
#define F32_MAX (__FLT_MAX__)
#define F64_MIN (-__DBL_MAX__)
#define F64_MAX (__DBL_MAX__)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * SAFE CASTING UTILITIES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_cast_to_u8(val)                                                                                            \
  ({                                                                                                                   \
    nya_assert((val) >= U8_MIN && (val) <= U8_MAX);                                                                    \
    (u8)(val);                                                                                                         \
  })

#define nya_cast_to_u16(val)                                                                                           \
  ({                                                                                                                   \
    nya_assert((val) >= U16_MIN && (val) <= U16_MAX);                                                                  \
    (u16)(val);                                                                                                        \
  })

#define nya_cast_to_u32(val)                                                                                           \
  ({                                                                                                                   \
    nya_assert((val) >= U32_MIN && (val) <= U32_MAX);                                                                  \
    (u32)(val);                                                                                                        \
  })

#define nya_cast_to_u64(val)                                                                                           \
  ({                                                                                                                   \
    nya_assert((val) >= U64_MIN && (val) <= U64_MAX);                                                                  \
    (u64)(val);                                                                                                        \
  })

#define nya_cast_to_s8(val)                                                                                            \
  ({                                                                                                                   \
    nya_assert((val) >= S8_MIN && (val) <= S8_MAX);                                                                    \
    (s8)(val);                                                                                                         \
  })

#define nya_cast_to_s16(val)                                                                                           \
  ({                                                                                                                   \
    nya_assert((val) >= S16_MIN && (val) <= S16_MAX);                                                                  \
    (s16)(val);                                                                                                        \
  })

#define nya_cast_to_s32(val)                                                                                           \
  ({                                                                                                                   \
    nya_assert((val) >= S32_MIN && (val) <= S32_MAX);                                                                  \
    (s32)(val);                                                                                                        \
  })

#define nya_cast_to_s64(val)                                                                                           \
  ({                                                                                                                   \
    nya_assert((val) >= S64_MIN && (val) <= S64_MAX);                                                                  \
    (s64)(val);                                                                                                        \
  })

#define nya_cast_to_f16(val)                                                                                           \
  ({                                                                                                                   \
    nya_assert((val) >= F16_MIN && (val) <= F16_MAX);                                                                  \
    (f16)(val);                                                                                                        \
  })

#define nya_cast_to_f32(val)                                                                                           \
  ({                                                                                                                   \
    nya_assert((val) >= F32_MIN && (val) <= F32_MAX);                                                                  \
    (f32)(val);                                                                                                        \
  })

#define nya_cast_to_f64(val)                                                                                           \
  ({                                                                                                                   \
    nya_assert((val) >= F64_MIN && (val) <= F64_MAX);                                                                  \
    (f64)(val);                                                                                                        \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FORMATTING UTILITIES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

// use like: nya_debug("number: "FMTu64, number);
#define FMTb8  "%" PRIu8
#define FMTb16 "%" PRIu16
#define FMTb32 "%" PRIu32
#define FMTb64 "%" PRIu64
#define FMTu8  "%" PRIu8
#define FMTu16 "%" PRIu16
#define FMTu32 "%" PRIu32
#define FMTu64 "%" PRIu64
#define FMTs8  "%" PRId8
#define FMTs16 "%" PRId16
#define FMTs32 "%" PRId32
#define FMTs64 "%" PRId64
#define FMTf16 "%e"
#define FMTf32 "%f"
#define FMTf64 "%lf"
