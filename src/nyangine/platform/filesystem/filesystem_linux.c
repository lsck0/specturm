#include <fcntl.h>
#include <sys/stat.h>

#include "nyangine/nyangine.h"

b8 nya_filesystem_is_file(NYA_ConstCString path) {
  struct stat path_stat;
  if (stat(path, &path_stat) != 0) return false;

  return S_ISREG(path_stat.st_mode);
}

b8 nya_filesystem_file_move(NYA_ConstCString source, NYA_ConstCString destination) {
  return rename(source, destination) == 0;
}

b8 nya_filesystem_file_copy(NYA_ConstCString source, NYA_ConstCString destination) {
  NYA_GUARDED_BY(close) s32 source_fd = open(source, O_RDONLY);
  if (source_fd < 0) return false;

  NYA_GUARDED_BY(close) s32 destination_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (destination_fd < 0) return false;

  NYA_GUARDED_BY(nya_arena_destroy) NYA_Arena arena  = nya_arena_new();
  NYA_String                                  buffer = nya_string_new(&arena);

  b8 ok;

  ok = nya_fd_read(source_fd, &buffer);
  if (!ok) return false;

  ok = nya_fd_write(destination_fd, &buffer);
  if (!ok) return false;

  return true;
}

b8 nya_filesystem_file_delete(NYA_ConstCString path) {
  return remove(path) == 0;
}

b8 nya_filesystem_file_last_modified(NYA_ConstCString path, OUT u64* out_timestamp) {
  struct stat path_stat;
  if (stat(path, &path_stat) != 0) return false;

  if (out_timestamp) {
    *out_timestamp = (u64)path_stat.st_mtim.tv_sec * 1000ULL + (u64)(path_stat.st_mtim.tv_nsec / 1000000ULL);
  }

  return true;
}
