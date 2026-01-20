#include "gnyame/gnyame.h"

#include "nyangine/base/base_keys.h"

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

  // if (nya_input_is_key_just_pressed(NYA_KEY_6)) nya_info("INITIALIZE 67");
  // if (nya_input_is_key_pressed(NYA_KEY_6)) nya_info("67");
  // if (nya_input_is_key_just_released(NYA_KEY_6)) nya_info("DEINITIALIZE 67");

  // nya_info("Mouse Position: " FMTf32x2, FMTf32x2_ARG(nya_input_get_mouse_position()));
  // nya_info("Mouse Delta: " FMTf32x2, FMTf32x2_ARG(nya_input_get_mouse_position_delta()));
  // nya_info("Mouse Wheel Scroll: " FMTf32x2, FMTf32x2_ARG(nya_input_get_mouse_wheel_scroll()));

  // if (nya_input_is_mouse_button_just_pressed(NYA_MOUSE_BUTTON_LEFT)) nya_info("Left Mouse Button Just Pressed");
  // if (nya_input_is_mouse_button_pressed(NYA_MOUSE_BUTTON_LEFT)) nya_info("Left Mouse Button Pressed");
  // if (nya_input_is_mouse_button_just_released(NYA_MOUSE_BUTTON_LEFT)) nya_info("Left Mouse Button Just Released");
}

void on_event(NYA_Window* window, NYA_Event* event) {
  nya_unused(window, event);
}
