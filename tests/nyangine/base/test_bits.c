#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

typedef enum {
  SAMPLE_FLAG_A = 1 << 0,
  SAMPLE_FLAG_B = 1 << 1,
  SAMPLE_FLAG_C = 1 << 2,
  SAMPLE_FLAG_D = 1 << 3,
} SAMPLE_FLAGS;

s32 main(void) {
  // test bit utilities
  u64 value = 0;
  nya_bit_set(value, BIT5);
  nya_assert(value == 0b10000);
  nya_assert(nya_bit_check(value, BIT5));
  nya_bit_unset(value, BIT5);
  nya_assert(!nya_bit_check(value, BIT5));
  nya_assert(value == 0);
  nya_bit_toggle(value, BIT5);
  nya_assert(value == 0b10000);
  nya_bit_toggle(value, BIT5);
  nya_assert(value == 0);

  // test bitmask utilities
  value = 0;
  nya_bitmask_set(value, BITMASK3);
  nya_assert(value == 0b111);
  nya_assert(nya_bitmask_check(value, BITMASK3));
  nya_bitmask_unset(value, BITMASK3);
  nya_assert(!nya_bitmask_check(value, BITMASK3));
  nya_assert(value == 0);
  nya_bitmask_toggle(value, BITMASK3);
  nya_assert(value == 0b111);
  nya_bitmask_toggle(value, BITMASK3);
  nya_assert(value == 0);

  // test bit flag utilities
  SAMPLE_FLAGS flags = 0;
  nya_flag_set(flags, SAMPLE_FLAG_A);
  nya_assert(nya_flag_check(flags, SAMPLE_FLAG_A));
  nya_assert(!nya_flag_check(flags, SAMPLE_FLAG_B));
  nya_assert(!nya_flag_check(flags, SAMPLE_FLAG_C));
  nya_flag_unset(flags, SAMPLE_FLAG_A);
  nya_assert(!nya_flag_check(flags, SAMPLE_FLAG_A));
  nya_assert(!nya_flag_check(flags, SAMPLE_FLAG_B));
  nya_assert(!nya_flag_check(flags, SAMPLE_FLAG_C));
  nya_flag_toggle(flags, SAMPLE_FLAG_A);
  nya_assert(nya_flag_check(flags, SAMPLE_FLAG_A));
  nya_assert(!nya_flag_check(flags, SAMPLE_FLAG_B));
  nya_assert(!nya_flag_check(flags, SAMPLE_FLAG_C));
  nya_flag_toggle(flags, SAMPLE_FLAG_B);
  nya_assert(nya_flag_check(flags, SAMPLE_FLAG_A));
  nya_assert(nya_flag_check(flags, SAMPLE_FLAG_B));
  nya_assert(!nya_flag_check(flags, SAMPLE_FLAG_C));
  nya_assert(!nya_flag_equals(flags, SAMPLE_FLAG_C));
  nya_assert(nya_flag_equals(flags, SAMPLE_FLAG_A | SAMPLE_FLAG_B));

  return 0;
}
