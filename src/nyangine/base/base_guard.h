#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_attributes.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define NYA_DEFINE_CLEANUP_FN(name, type, var_name, code)                                                              \
  __attr_unused void __cleanup_##name(type const* p) {                                                                 \
    if (!p) return;                                                                                                    \
    type var_name = *p;                                                                                                \
    code;                                                                                                              \
  }

/**
 * This is essentially poor mans RAII / defer for C.
 *
 * EXAMPLE USAGE:
 *
 * NYA_GUARDED_BY(close) s32 source_fd = open(source, O_RDONLY);
 * if (source_fd < 0) return false;
 *
 * NYA_GUARDED_BY(close) s32 destination_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
 * if (destination_fd < 0) return false;
 *
 * NYA_GUARDED_BY(nya_arena_destroy) NYA_Arena arena  = nya_arena_create();
 * NYA_String                                  buffer = nya_string_create(&arena);
 *
 * b8 ok;
 *
 * ok = nya_fd_read(source_fd, &buffer);
 * if (!ok) return false;
 *
 * ok = nya_fd_write(destination_fd, &buffer);
 * if (!ok) return false;
 *
 * return true;
 * */
#define NYA_GUARDED_BY(cleanup_fn) __attr_cleanup(__cleanup_##cleanup_fn)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * NON NYA* CLEANUP FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_DEFINE_CLEANUP_FN(
    close,
    int,
    fd,
    if (fd >= 0) {
      close(fd);
      fd = -1;
    }
)

NYA_DEFINE_CLEANUP_FN(
    fclose,
    FILE*,
    f,
    if (f) {
      (void)fclose(f);
      f = nullptr;
    }
)
