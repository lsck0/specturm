#include "nyangine/base/base_logging.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "nyangine/base/base.h"
#include "nyangine/base/base_assert.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

static NYA_LogLevel log_level_current = NYA_LOG_LEVEL_INFO;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_LogLevel nya_log_level_get(void) {
  return log_level_current;
}

void nya_log_level_set(NYA_LogLevel level) {
  log_level_current = level;
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void _nya_log_message(NYA_LogLevel level, const char* function, const char* file, u32 line, const char* format, ...) {
  if (level < log_level_current) return;

  static const char* log_level_strings[] = {
      [NYA_LOG_LEVEL_DEBUG] = "DEBUG",
      [NYA_LOG_LEVEL_INFO]  = "INFO",
      [NYA_LOG_LEVEL_WARN]  = "WARN",
      [NYA_LOG_LEVEL_ERROR] = "ERROR",
      [NYA_LOG_LEVEL_PANIC] = "PANIC",
  };
  printf("[%s] %s (%s:%u): ", log_level_strings[level], function, file, line);

  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  printf("\n");

  if (nya_unlikely(level == NYA_LOG_LEVEL_PANIC)) {
    if (NYA_IS_DEBUG) {
      __builtin_trap();
    } else {
      abort();
    }
  }
}
