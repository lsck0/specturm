#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  u64 t1 = nya_clock_get_timestamp_ms();
  nya_assert(t1 > 0);

  u64 t2 = nya_clock_get_timestamp_ms();
  nya_assert(t2 >= t1);

  return 0;
}
