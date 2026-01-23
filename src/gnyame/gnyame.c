#include "gnyame/gnyame.h"

void on_create(void);
void on_destroy(void);
void on_update(NYA_Window* window, f32 delta_time);
void on_event(NYA_Window* window, NYA_Event* event);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ENTRY POINT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gnyame_init(s32 argc, char** argv) {
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
          .id         = "main_layer",
          .enabled    = true,
          .on_create  = on_create,
          .on_destroy = on_destroy,
          .on_update  = on_update,
          .on_event   = on_event,
          .on_render  = nullptr,
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

void on_create(void) {
  nya_info("gnyame (v%s) started", NYA_VERSION);
}

void on_destroy(void) {
  nya_info("gnyame shutting down");
}

void on_update(NYA_Window* window, f32 delta_time) {
  nya_unused(window);
  nya_unused(delta_time);

  // NYA_Asset* asset = nya_asset_load_and_get(NYA_ASSETS_TEXTS_HELLO_TXT);
  // printf("%.*s", (s32)asset->size, (char*)asset->data);
}

void on_event(NYA_Window* window, NYA_Event* event) {
  nya_unused(window, event);
}
