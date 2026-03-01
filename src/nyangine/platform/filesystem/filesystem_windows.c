#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "nyangine/nyangine.h"

b8 nya_filesystem_exists(NYA_ConstCString path) {
  DWORD attributes = GetFileAttributesA(path);
  if (attributes == INVALID_FILE_ATTRIBUTES) return false;
  return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

NYA_Result nya_filesystem_move(NYA_ConstCString old_path, NYA_ConstCString new_path) {
  if (!MoveFileA(old_path, new_path)) return nya_err(NYA_ERROR_IO, "Failed to move '%s' to '%s'.", old_path, new_path);
  return NYA_OK;
}

NYA_Result nya_filesystem_copy(NYA_ConstCString source, NYA_ConstCString destination) {
  if (!CopyFileA(source, destination, FALSE)) return nya_err(NYA_ERROR_IO, "Failed to copy '%s' to '%s'.", source, destination);
  return NYA_OK;
}

NYA_Result nya_filesystem_delete(NYA_ConstCString path) {
  if (!DeleteFileA(path)) return nya_err(NYA_ERROR_IO, "Failed to delete '%s'.", path);
  return NYA_OK;
}

NYA_Result nya_filesystem_last_modified(NYA_ConstCString path, OUT u64* out_timestamp) {
  WIN32_FILE_ATTRIBUTE_DATA file_info;
  if (!GetFileAttributesExA(path, GetFileExInfoStandard, &file_info)) return nya_err(NYA_ERROR_NOT_FOUND, "Failed to get attributes for '%s'.", path);

  if (out_timestamp) {
    FILETIME       ft = file_info.ftLastWriteTime;
    ULARGE_INTEGER ull;
    ull.LowPart    = ft.dwLowDateTime;
    ull.HighPart   = ft.dwHighDateTime;
    *out_timestamp = (ull.QuadPart / 10000000ULL) - 11644473600ULL;
  }

  return NYA_OK;
}
