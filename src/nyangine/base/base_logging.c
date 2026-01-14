#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL NYA_LogLevel  _nya_log_level_current = NYA_LOG_LEVEL_INFO;
NYA_INTERNAL NYA_PanicHook _nya_panic_hook        = nullptr;
NYA_INTERNAL jmp_buf*      _nya_panic_prevent_jmp = nullptr;
NYA_INTERNAL b8            _nya_panic_prevented   = false;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_LogLevel nya_log_level_get(void) {
  return _nya_log_level_current;
}

void nya_log_level_set(NYA_LogLevel level) {
  _nya_log_level_current = level;
}

void nya_panic_hook_set(NYA_PanicHook hook) {
  _nya_panic_hook = hook;
}
void nya_panic_prevent_set(jmp_buf* jmp) {
  _nya_panic_prevent_jmp = jmp;
}

b8 nya_panic_prevent_happened(void) {
  b8 prevented           = _nya_panic_prevented;
  _nya_panic_prevented   = false;
  _nya_panic_prevent_jmp = nullptr;

  return prevented;
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void _nya_log_message(NYA_LogLevel level, const char* function, const char* file, u32 line, const char* format, ...) {
  if (level < _nya_log_level_current) return;

  static const char* log_level_strings[] = {
      [NYA_LOG_LEVEL_DEBUG] = "DEBUG",
      [NYA_LOG_LEVEL_INFO]  = "INFO",
      [NYA_LOG_LEVEL_WARN]  = "WARN",
      [NYA_LOG_LEVEL_ERROR] = "ERROR",
      [NYA_LOG_LEVEL_PANIC] = "PANIC",
  };
  if (nya_unlikely(level == NYA_LOG_LEVEL_PANIC) && _nya_panic_prevent_jmp) {
    printf("[PREVENTED PANIC] %s (%s:%u): ", function, file, line);
  } else {
    printf("[%s] %s (%s:%u): ", log_level_strings[level], function, file, line);
  }

  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  printf("\n");

  if (nya_unlikely(level == NYA_LOG_LEVEL_PANIC)) {
    if (_nya_panic_prevent_jmp) {
      _nya_panic_prevented = true;
      longjmp(*_nya_panic_prevent_jmp, 1);
    }

    if (_nya_panic_hook) {
      va_list args;
      va_start(args, format);
      b8 prevent_crash = _nya_panic_hook(function, file, line, format, args);
      va_end(args);

      if (prevent_crash) return;
    }

    if (NYA_IS_DEBUG) __builtin_trap();
    abort();
  }
}
