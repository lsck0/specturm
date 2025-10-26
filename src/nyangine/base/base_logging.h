#pragma once
// clang-format off

#include "nyangine/base/base.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_ints.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef enum {
  NYA_LOG_LEVEL_DEBUG = 0,
  NYA_LOG_LEVEL_INFO,
  NYA_LOG_LEVEL_WARN,
  NYA_LOG_LEVEL_ERROR,
  NYA_LOG_LEVEL_PANIC,
  NYA_LOG_LEVEL_COUNT,
} NYA_LogLevel;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS AND MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_debug(format, ...) _nya_log_message(NYA_LOG_LEVEL_DEBUG, __FUNCTION__, __FILE__, __LINE__, format __VA_OPT__(, __VA_ARGS__))
#define nya_info(format, ...)  _nya_log_message(NYA_LOG_LEVEL_INFO,  __FUNCTION__, __FILE__, __LINE__, format __VA_OPT__(, __VA_ARGS__))
#define nya_warn(format, ...)  _nya_log_message(NYA_LOG_LEVEL_WARN,  __FUNCTION__, __FILE__, __LINE__, format __VA_OPT__(, __VA_ARGS__))
#define nya_error(format, ...) _nya_log_message(NYA_LOG_LEVEL_ERROR, __FUNCTION__, __FILE__, __LINE__, format __VA_OPT__(, __VA_ARGS__))
#define nya_panic(format, ...) _nya_log_message(NYA_LOG_LEVEL_PANIC, __FUNCTION__, __FILE__, __LINE__, format __VA_OPT__(, __VA_ARGS__)); __builtin_unreachable();

NYA_API extern NYA_LogLevel nya_log_level_get(void);
NYA_API extern void         nya_log_level_set(NYA_LogLevel level);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * INTERNALS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API extern void _nya_log_message(NYA_LogLevel level, const char* function, const char* file, u32 line, const char* format, ...) __attr_fmt_printf(5, 6);
