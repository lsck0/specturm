#include "nyangine/nyangine.h"

u64 nya_clock_get_timestamp_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (u64)(ts.tv_sec) * 1000 + (u64)(ts.tv_nsec) / 1000000;
}
