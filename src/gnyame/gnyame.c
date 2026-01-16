#include "gnyame/gnyame.h"

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

  nya_app_init(
      &NYA_APP_INSTANCE,
      (NYA_AppConfig){
          .time_step_ms     = 15,
          .frame_rate_limit = 120,
          .vsync_enabled    = false,
      }
  );

  nya_window_new(&NYA_APP_INSTANCE, "gnyame", 1280, 720, NYA_WINDOW_RESIZABLE, "window");
  nya_window_new(&NYA_APP_INSTANCE, "gnyame2", 1280, 720, NYA_WINDOW_RESIZABLE, "window2");
  nya_layer_push(
      &NYA_APP_INSTANCE,
      "window",
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
