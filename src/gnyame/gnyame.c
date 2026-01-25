#include "gnyame/gnyame.h"

void on_update(NYA_Window* window, f32 delta_time);
void on_event(NYA_Window* window, NYA_Event* event);
void on_render(NYA_Window* window);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ENTRY POINT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gnyame_init(s32 argc, NYA_CString* argv) {
  nya_unused(argc, argv);

  nya_app_init((NYA_AppConfig){
      .time_step_ms     = 15,
      .frame_rate_limit = 120,
      .vsync_enabled    = false,
  });

  nya_window_create("gnyame", 1280, 720, NYA_WINDOW_RESIZABLE, "window");
  nya_layer_push(
      "window",
      (NYA_Layer){
          .id        = "main_layer",
          .enabled   = true,
          .on_update = on_update,
          .on_event  = on_event,
          .on_render = on_render,
      }
  );
}

void gnyame_run(void) {
  nya_app_run();
}

void gnyame_deinit(void) {
  nya_app_deinit();
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * LAYERS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void on_update(NYA_Window* window, f32 delta_time) {
  nya_unused(window);
  nya_unused(delta_time);
}

void on_event(NYA_Window* window, NYA_Event* event) {
  nya_unused(window, event);
}

void on_render(NYA_Window* window) {
  nya_unused(window);
}
