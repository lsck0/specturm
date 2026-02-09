#include "gnyame/gnyame.h"

#include "gnyame/layers/layer_game.c"
#include "gnyame/layers/layer_ui.c"
#include "gnyame/layers/layers.c"
#include "gnyame/windows.c"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * GNYAME INIT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gnyame_init(s32 argc, NYA_CString* argv) {
  nya_unused(argc, argv);

  nya_app_init((NYA_AppConfig){
      .time_step_ns        = nya_time_ms_to_ns(15),
      .frame_rate_limit    = 60,
      .vsync_enabled       = false,
      .max_concurrent_jobs = 4,
  });

  gny_layers_init();
  gny_window_main_create();
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * GNYAME RUN
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gnyame_run(void) {
  nya_app_run();
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * GNYAME DEINIT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gnyame_deinit(void) {
  nya_app_deinit();
}
