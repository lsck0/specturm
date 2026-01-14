#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  // test math utilities
  nya_assert(nya_min(3, 5) == 3);
  nya_assert(nya_max(3, 5) == 5);
  nya_assert(nya_clamp(7, 0, 5) == 5);
  nya_assert(nya_clamp(-2, 0, 5) == 0);
  nya_assert(nya_clamp(3, 0, 5) == 3);
  nya_assert(nya_lerp(0.0, 10.0, 0.0) == 0.0);
  nya_assert(nya_lerp(0.0, 10.0, 0.5) == 5.0);
  nya_assert(nya_lerp(0.0, 10.0, 1.0) == 10.0);

  nya_assert_panic(nya_lerp(0.0, 10.0, -0.1));

  return 0;
}
