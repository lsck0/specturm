#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Basic timestamp retrieval
  // ─────────────────────────────────────────────────────────────────────────────
  u64 t1 = nya_clock_get_timestamp_ms();
  nya_assert(t1 > 0);

  u64 t2 = nya_clock_get_timestamp_ms();
  nya_assert(t2 >= t1);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Timestamps are monotonically increasing
  // ─────────────────────────────────────────────────────────────────────────────
  u64 prev = nya_clock_get_timestamp_ms();
  for (u32 i = 0; i < 100; ++i) {
    u64 curr = nya_clock_get_timestamp_ms();
    nya_assert(curr >= prev);
    prev = curr;
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Elapsed time measurement (spin wait)
  // ─────────────────────────────────────────────────────────────────────────────
  u64 start = nya_clock_get_timestamp_ms();
  // Spin until at least 10ms have passed
  while ((nya_clock_get_timestamp_ms() - start) < 10) {
    // busy wait
  }
  u64 end = nya_clock_get_timestamp_ms();
  u64 elapsed = end - start;

  // Should have elapsed at least 10ms
  nya_assert(elapsed >= 10);
  nya_assert(elapsed < 1000);  // Sanity check

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Multiple consecutive calls produce consistent results
  // ─────────────────────────────────────────────────────────────────────────────
  u64 times[10];
  for (u32 i = 0; i < 10; ++i) {
    times[i] = nya_clock_get_timestamp_ms();
  }
  for (u32 i = 1; i < 10; ++i) {
    nya_assert(times[i] >= times[i - 1]);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Timestamp is within reasonable range (since 2020)
  // ─────────────────────────────────────────────────────────────────────────────
  u64 now = nya_clock_get_timestamp_ms();
  u64 year_2020_ms = 1577836800000ULL;  // Jan 1, 2020 in milliseconds
  u64 year_2050_ms = 2524608000000ULL;  // Jan 1, 2050 in milliseconds
  nya_assert(now > year_2020_ms);
  nya_assert(now < year_2050_ms);

  return 0;
}
