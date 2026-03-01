#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_Result nya_result_from_errno(void) {
  NYA_Error error;

  switch (errno) {
    case ENOENT:
    case ESRCH:
    case ENXIO:
    case ENODEV:
    case ENOTDIR:
    case ENAMETOOLONG: error = NYA_ERROR_NOT_FOUND; break;

    case EACCES:
    case EPERM:
    case EROFS:        error = NYA_ERROR_PERMISSION_DENIED; break;

    case ENOMEM:       error = NYA_ERROR_OUT_OF_MEMORY; break;

    default:           error = NYA_ERROR_GENERIC; break;
  }

  NYA_Result result = { .error = error };

  (void)snprintf(result.message, sizeof(result.message), "%s (errno %d)", strerror(errno), errno);

  return result;
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_Result _nya_result(NYA_Error error, NYA_ConstCString fmt, ...) {
  nya_assert(fmt != nullptr);
  nya_assert(error < NYA_ERROR_COUNT);

  NYA_Result result = { .error = error };

  va_list args;
  va_start(args, fmt);
  (void)vsnprintf(result.message, sizeof(result.message), fmt, args);
  va_end(args);

  return result;
}
