#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {

  nya_assert_panic({ nya_assert(1 == 2); });

  return 0;
}
