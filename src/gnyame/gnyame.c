#include "gnyame/gnyame.h"

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

void on_create(void);
void on_destroy(void);
void on_update(NYA_Window* window, f32 delta_time);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ENTRY POINT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL NYA_App NYA_APP_INSTANCE;

void gnyame_setup(s32 argc, char** argv) {
  nya_unused(argc, argv);

  NYA_App app = nya_app_new((NYA_AppConfig){
      .time_step_ms     = 15,
      .frame_rate_limit = 120,
      .vsync_enabled    = false,
  });

  nya_window_new(&app, "gnyame", 1280, 720, NYA_WINDOW_RESIZABLE, "main_window");
  nya_window_layer_push(
      &app,
      "main_window",
      (NYA_Layer){
          .id         = "main_layer",
          .enabled    = true,
          .on_create  = on_create,
          .on_destroy = on_destroy,
          .on_update  = on_update,
          .on_event   = nullptr,
          .on_render  = nullptr,
      }
  );

  NYA_APP_INSTANCE = app;
}

void gnyame_run(void) {
  nya_app_run(&NYA_APP_INSTANCE);
}

void gnyame_shutdown(void) {
  nya_app_destroy(&NYA_APP_INSTANCE);
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * HOT RELOADING UTILITIES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if NYA_IS_DEBUG
NYA_App gnyame_get_appstate() {
  return NYA_APP_INSTANCE;
}

void gnyame_set_appstate(NYA_App app) {
  NYA_APP_INSTANCE = app;
}

void gnyame_stop_game_loop(void) {
  NYA_APP_INSTANCE.should_quit_game_loop = true;
}
#endif // NYA_IS_DEBUG

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * LAYERS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void on_create(void) {
  nya_info("gnyame (v%s) started", NYA_VERSION);
}

void on_destroy(void) {
  nya_info("gnyame shutting down");
}

void on_update(NYA_Window* window, f32 delta_time) {
  nya_unused(window);
  nya_unused(delta_time);

  nya_info("gnyame updating");
}
