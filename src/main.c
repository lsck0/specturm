#include "gnyame/gnyame.c"
#include "gnyame/gnyame.h"

s32 main(s32 argc, char** argv) {
  gnyame_setup(argc, argv);
  gnyame_run();
  gnyame_shutdown();
  return 0;
}
