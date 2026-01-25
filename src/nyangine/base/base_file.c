#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FD FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

b8 nya_fd_read(s32 fd, OUT NYA_String* out_content) {
  nya_assert(out_content != nullptr);

  char buffer[4096];
  u64  n;
  while ((n = read(fd, buffer, sizeof(buffer))) > 0) { /**/
    nya_string_extend(out_content, &(NYA_String){ .items = (u8*)buffer, .length = n });
  }

  return n == 0;
}

b8 nya_fd_write(s32 fd, const NYA_String* content) __attr_overloaded {
  nya_assert(content != nullptr);

  u64 total_written = 0;
  while (total_written < content->length) {
    s64 written = write(fd, content->items + total_written, content->length - total_written);
    if (written <= 0) return false;
    total_written += written;
  }

  return true;
}

b8 nya_fd_write(s32 fd, NYA_ConstCString content) __attr_overloaded {
  nya_assert(content != nullptr);

  return nya_fd_write(
      fd,
      &((NYA_String){
          .items  = (u8*)content,
          .length = strlen(content),
      })
  );
}

b8 nya_fd_append(s32 fd, const NYA_String* content) __attr_overloaded {
  nya_assert(content != nullptr);

  if (lseek(fd, 0, SEEK_END) == -1) return false;

  return nya_fd_write(fd, content);
}

b8 nya_fd_append(s32 fd, NYA_ConstCString content) __attr_overloaded {
  nya_assert(content != nullptr);

  return nya_fd_append(
      fd,
      &((NYA_String){
          .items  = (u8*)content,
          .length = strlen(content),
      })
  );
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FILE FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * READ
 * ─────────────────────────────────────────────────────────
 */

b8 nya_file_read(const char* path, OUT NYA_String* out_content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(out_content != nullptr);

  FILE* file = fopen(path, "rb");
  if (!file) return false;

  s32 fd     = fileno(file);
  b8  ok     = nya_fd_read(fd, out_content);
  b8  closed = (fclose(file) == 0);

  return ok && closed;
}

b8 nya_file_read(const NYA_String* path, OUT NYA_String* out_content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(out_content != nullptr);

  NYA_CString c_path = nya_alloca((path->length + 1) * sizeof(char));
  memcpy((void*)c_path, path->items, path->length);
  c_path[path->length] = '\0';

  return nya_file_read(c_path, out_content);
}

/*
 * ─────────────────────────────────────────────────────────
 * WRITE
 * ─────────────────────────────────────────────────────────
 */

b8 nya_file_write(const char* path, const NYA_String* content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(content != nullptr);

  FILE* file = fopen(path, "wb");
  if (!file) return false;

  s32 fd     = fileno(file);
  b8  ok     = nya_fd_write(fd, content);
  b8  closed = (fclose(file) == 0);

  return ok && closed;
}

b8 nya_file_write(const NYA_String* path, const NYA_String* content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(content != nullptr);

  NYA_CString c_path = nya_alloca((path->length + 1) * sizeof(char));
  memcpy((void*)c_path, path->items, path->length);
  c_path[path->length] = '\0';

  return nya_file_write(c_path, content);
}

b8 nya_file_write(const char* path, NYA_ConstCString content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(content != nullptr);

  return nya_file_write(
      path,
      &((NYA_String){
          .items  = (u8*)content,
          .length = strlen(content),
      })
  );
}

b8 nya_file_write(const NYA_String* path, NYA_ConstCString content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(content != nullptr);

  NYA_CString c_path = nya_alloca((path->length + 1) * sizeof(char));
  memcpy((void*)c_path, path->items, path->length);
  c_path[path->length] = '\0';

  return nya_file_write(
      c_path,
      &((NYA_String){
          .items  = (u8*)content,
          .length = strlen(content),
      })
  );
}

/*
 * ─────────────────────────────────────────────────────────
 * APPEND
 * ─────────────────────────────────────────────────────────
 */

b8 nya_file_append(const char* path, const NYA_String* content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(content != nullptr);

  FILE* file = fopen(path, "ab");
  if (!file) return false;

  s32 fd     = fileno(file);
  b8  ok     = nya_fd_append(fd, content);
  b8  closed = (fclose(file) == 0);

  return ok && closed;
}

b8 nya_file_append(const NYA_String* path, const NYA_String* content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(content != nullptr);

  NYA_CString c_path = nya_alloca((path->length + 1) * sizeof(char));
  memcpy((void*)c_path, path->items, path->length);
  c_path[path->length] = '\0';

  return nya_file_append(c_path, content);
}

b8 nya_file_append(const char* path, NYA_ConstCString content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(content != nullptr);

  return nya_file_append(
      path,
      &((NYA_String){
          .items  = (u8*)content,
          .length = strlen(content),
      })
  );
}

b8 nya_file_append(const NYA_String* path, NYA_ConstCString content) __attr_overloaded {
  nya_assert(path != nullptr);
  nya_assert(content != nullptr);

  NYA_CString c_path = nya_alloca((path->length + 1) * sizeof(char));
  memcpy((void*)c_path, path->items, path->length);
  c_path[path->length] = '\0';

  return nya_file_append(
      c_path,
      &((NYA_String){
          .items  = (u8*)content,
          .length = strlen(content),
      })
  );
}
