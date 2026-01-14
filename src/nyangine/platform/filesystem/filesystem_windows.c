#include <windows.h>

#include "nyangine/nyangine.h"

b8 nya_filesystem_exists(NYA_ConstCString path) {
  DWORD attributes = GetFileAttributesA(path);
  if (attributes == INVALID_FILE_ATTRIBUTES) return false;
  return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

b8 nya_filesystem_move(NYA_ConstCString old_path, NYA_ConstCString new_path) {
  return MoveFileA(old_path, new_path) != 0;
}

b8 nya_filesystem_copy(NYA_ConstCString source, NYA_ConstCString destination) {
  return CopyFileA(source, destination, FALSE) != 0;
}

b8 nya_filesystem_delete(NYA_ConstCString path) {
  return DeleteFileA(path) != 0;
}

b8 nya_filesystem_last_modified(NYA_ConstCString path, OUT u64* out_timestamp) {
  WIN32_FILE_ATTRIBUTE_DATA file_info;
  if (!GetFileAttributesExA(path, GetFileExInfoStandard, &file_info)) return false;

  if (out_timestamp) {
    FILETIME       ft = file_info.ftLastWriteTime;
    ULARGE_INTEGER ull;
    ull.LowPart    = ft.dwLowDateTime;
    ull.HighPart   = ft.dwHighDateTime;
    *out_timestamp = (ull.QuadPart / 10000000ULL) - 11644473600ULL;
  }

  return true;
}
