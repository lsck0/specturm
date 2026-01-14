// clang-format off
#pragma once

#include "nyangine/base/base_logging.h"

#define nya_likely(expr)   __builtin_expect(!!(expr), 1)
#define nya_unlikely(expr) __builtin_expect(!!(expr), 0)

#define nya_assert_type_match(a, b) static_assert(__builtin_types_compatible_p(typeof(a), typeof(b)), "Incompatible types.")

/**
 * Usage:
 * nya_assert(condition)
 * nya_assert(condition, message)
 * nya_assert(condition, format, ...) with a max. of 10 format arguments
 * */
#define nya_assert(...)     _nya_assert(__VA_ARGS__)
#define nya_todo()          nya_assert(0, "Todo"); __builtin_unreachable();
#define nya_unimplemented() nya_assert(0, "Unimplemented"); __builtin_unreachable();
#define nya_unreachable()   nya_assert(0, "Unreachable"); __builtin_unreachable();
#define nya_unused(...)     ((void)(0, __VA_ARGS__))
/**
 * Usage:
 * nya_assert_panic(a_function_call());
 *
 * nya_assert_panic({
 *  // code that is expected to panic
 * });
 * */
#define nya_assert_panic(code) {                                            \
    nya_panic_prevent_next();                                               \
    code;                                                                   \
    nya_assert(nya_panic_prevent_happened(), "Expected to catch a panic."); \
  }

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * INTERNALS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#ifdef NYA_NO_ASSERT
#define _nya_assert(...) ((void)0)
#else
#define _nya_assert(...)  _NYA_PICK_ASSERT(__VA_ARGS__, _NYA_ASSERT3, _NYA_ASSERT3, _NYA_ASSERT3, _NYA_ASSERT3, _NYA_ASSERT3, _NYA_ASSERT3, _NYA_ASSERT3, _NYA_ASSERT3, _NYA_ASSERT3, _NYA_ASSERT3, _NYA_ASSERT2, _NYA_ASSERT1)(__VA_ARGS__)
#define _NYA_PICK_ASSERT(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, NAME, ...) NAME
#define _NYA_ASSERT1(condition)              if (nya_unlikely(!(condition))) { nya_panic("Assertion failed: %s", #condition); }
#define _NYA_ASSERT2(condition, message)     if (nya_unlikely(!(condition))) { nya_panic("Assertion failed: %s, %s", #condition, message); }
#define _NYA_ASSERT3(condition, format, ...) if (nya_unlikely(!(condition))) { nya_panic("Assertion failed: %s, " format, #condition, __VA_ARGS__); }
#endif // NYA_NO_ASSERT
