#include <windows.h>

#include "nyangine/nyangine.h"

u64 nya_clock_get_timestamp_ms(void) {
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);

  // Convert FILETIME to 64-bit
  u64 time = ((u64)ft.dwHighDateTime << 32) | ft.dwLowDateTime;

  // FILETIME is in 100-nanosecond intervals since January 1, 1601 (UTC)
  // Convert to milliseconds and adjust to Unix epoch (January 1, 1970)
  const u64 EPOCH_DIFFERENCE_MS = 11644473600000ULL;
  return (time / 10000) - EPOCH_DIFFERENCE_MS;
}
