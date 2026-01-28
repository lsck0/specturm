#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <sys/stat.h>

#include "nyangine/nyangine.h"

b8 nya_filesystem_exists(NYA_ConstCString path) {
  b8 ok;

  struct stat path_stat;
  ok = stat(path, &path_stat) == 0;
  if (!ok) return false;

  return S_ISREG(path_stat.st_mode);
}

b8 nya_filesystem_last_modified(NYA_ConstCString path, OUT u64* out_timestamp) {
  b8 ok;

  struct stat path_stat;
  ok = stat(path, &path_stat) == 0;
  if (!ok) return false;

  if (out_timestamp) { /**/
    *out_timestamp = (u64)path_stat.st_mtim.tv_sec * 1000ULL + (u64)(path_stat.st_mtim.tv_nsec / 1000000ULL);
  }

  return true;
}

b8 nya_filesystem_move(NYA_ConstCString source, NYA_ConstCString destination) {
  b8 ok;

  ok = rename(source, destination);
  if (ok != 0) return false;

  return true;
}

b8 nya_filesystem_copy(NYA_ConstCString source, NYA_ConstCString destination) {
  NYA_GUARDED_BY(close) s32 source_fd = open(source, O_RDONLY);
  if (source_fd < 0) return false;

  NYA_GUARDED_BY(close) s32 destination_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (destination_fd < 0) return false;

  NYA_GUARDED_BY(nya_arena_destroy) NYA_Arena* arena  = nya_arena_create();
  NYA_String*                                  buffer = nya_string_create(arena);

  b8 ok;

  ok = nya_fd_read(source_fd, buffer);
  if (!ok) return false;

  ok = nya_fd_write(destination_fd, buffer);
  if (!ok) return false;

  return true;
}

b8 nya_filesystem_delete(NYA_ConstCString path) {
  b8 ok;

  ok = remove(path);
  if (ok != 0) return false;

  return true;
}
