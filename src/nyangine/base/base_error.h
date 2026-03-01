/**
 * @file base_error.h
 *
 * Error handling and missing values.
 *
 * All fallible functions return `NYA_Result`. Use the provided macros to handle results:
 *
 * - `NYA_OK`             — Success result.
 * - `nya_err(code, ...)`  — Create an error result with a formatted message.
 * - `nya_try(expr)`       — Propagate errors: if `expr` fails, return its result from the calling function.
 * - `nya_expect(expr)`    — Assert success: if `expr` fails, panic with the error details.
 *
 * For missing values, derive a `Maybe` type:
 *
 * - `nya_derive_maybe(T)` — Generates `MaybeT` with `.has_value` and `.value`.
 * - `nya_some(T, val)`     — Wrap a value.
 * - `nya_none(T)`          — Empty.
 *
 * `nya_result_from_errno()` converts the current `errno` into a `NYA_Result` with the appropriate error code.
 * */

#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef enum NYA_Error    NYA_Error;
typedef struct NYA_Result NYA_Result;

enum NYA_Error {
  NYA_ERROR_NONE = 0,
  NYA_ERROR_GENERIC,
  NYA_ERROR_NOT_FOUND,
  NYA_ERROR_PERMISSION_DENIED,
  NYA_ERROR_OUT_OF_MEMORY,
  NYA_ERROR_PARSE_ERROR,
  NYA_ERROR_COUNT,
};

struct NYA_Result {
  NYA_Error error;
  char      message[512];
};

__attr_unused static NYA_ConstCString NYA_RESULT_NAME_MAP[NYA_ERROR_COUNT] = {
  [NYA_ERROR_NONE]              = "NONE",
  [NYA_ERROR_GENERIC]           = "GENERIC",
  [NYA_ERROR_NOT_FOUND]         = "NOT_FOUND",
  [NYA_ERROR_PERMISSION_DENIED] = "PERMISSION_DENIED",
  [NYA_ERROR_OUT_OF_MEMORY]     = "OUT_OF_MEMORY",
  [NYA_ERROR_PARSE_ERROR]       = "PARSE_ERROR",
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS AND MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * MISSING VALUES
 * ─────────────────────────────────────────────────────────
 */

#define nya_derive_maybe(type)                                                                                                                       \
  typedef struct {                                                                                                                                   \
    b8   has_value;                                                                                                                                  \
    type value;                                                                                                                                      \
  } Maybe##type

#define nya_none(type)      ((Maybe##type){ .has_value = false })
#define nya_some(type, val) ((Maybe##type){ .has_value = true, .value = (val) })

nya_derive_maybe(b8);
nya_derive_maybe(b16);
nya_derive_maybe(b32);
nya_derive_maybe(b64);
nya_derive_maybe(b128);
nya_derive_maybe(u8);
nya_derive_maybe(u16);
nya_derive_maybe(u32);
nya_derive_maybe(u64);
nya_derive_maybe(u128);
nya_derive_maybe(s8);
nya_derive_maybe(s16);
nya_derive_maybe(s32);
nya_derive_maybe(s64);
nya_derive_maybe(s128);
nya_derive_maybe(f16);
nya_derive_maybe(f32);
nya_derive_maybe(f64);
nya_derive_maybe(f128);

/*
 * ─────────────────────────────────────────────────────────
 * ERROR HANDLING
 * ─────────────────────────────────────────────────────────
 */

#define NYA_OK       ((NYA_Result){ .error = NYA_ERROR_NONE })
#define nya_err(...) _nya_err(__VA_ARGS__)

#define nya_try(expr)                                                                                                                                \
  do {                                                                                                                                               \
    NYA_Result _result = (expr);                                                                                                                     \
    if (_result.error != NYA_ERROR_NONE) {                                                                                                           \
      nya_error("Tried '%s' and got an error: '%s'. Propagating up.", #expr, NYA_RESULT_NAME_MAP[_result.error]);                                    \
      return _result;                                                                                                                                \
    }                                                                                                                                                \
  } while (0)

#define nya_expect(expr)                                                                                                                             \
  do {                                                                                                                                               \
    NYA_Result _result = (expr);                                                                                                                     \
    nya_assert(_result.error == NYA_ERROR_NONE, "Expected '%s' to succeed but got error '%s'.", #expr, NYA_RESULT_NAME_MAP[_result.error]);          \
  } while (0)

NYA_API NYA_EXTERN NYA_Result nya_result_from_errno(void) __attr_no_discard;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * INTERNAL
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

// clang-format off
#define _nya_err(...)  _NYA_PICK_ERR(__VA_ARGS__, _NYA_ERR3, _NYA_ERR3, _NYA_ERR3, _NYA_ERR3, _NYA_ERR3, _NYA_ERR3, _NYA_ERR3, _NYA_ERR3, _NYA_ERR3, _NYA_ERR3, _NYA_ERR2, _NYA_ERR1)(__VA_ARGS__)
#define _NYA_PICK_ERR(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, NAME, ...) NAME
#define _NYA_ERR1(code)              _nya_result(code, "")
#define _NYA_ERR2(code, message)     _nya_result(code, "%s", message)
#define _NYA_ERR3(code, format, ...) _nya_result(code, format, __VA_ARGS__)
// clang-format on

NYA_API NYA_EXTERN NYA_Result _nya_result(NYA_Error error, NYA_ConstCString fmt, ...) __attr_fmt_printf(2, 3) __attr_no_discard;
