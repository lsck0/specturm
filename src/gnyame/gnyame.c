#include "gnyame/gnyame.h"

void on_update(NYA_Window* window, f32 delta_time_s);
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
      .time_step_ns     = nya_time_ms_to_ns(15),
      .frame_rate_limit = 60,
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

  nya_asset_load((NYA_AssetLoadParameters){
      .type   = NYA_ASSET_TYPE_SHADER_VERTEX,
      .handle = NYA_ASSETS_SHADERS_SOURCE_SAMPLE_VERT_HLSL,
  });

  nya_asset_load((NYA_AssetLoadParameters){
      .type   = NYA_ASSET_TYPE_SHADER_FRAGMENT,
      .handle = NYA_ASSETS_SHADERS_SOURCE_SAMPLE_FRAG_HLSL,
  });
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

void on_update(NYA_Window* window, f32 delta_time_s) {
  nya_unused(window, delta_time_s);

  if (nya_input_is_key_just_pressed(NYA_KEY_A)) { nya_asset_release(NYA_ASSETS_TEXTS_HELLO_TXT); }

  if (nya_input_is_key_just_pressed(NYA_KEY_B)) {
    nya_asset_load((NYA_AssetLoadParameters){
        .type   = NYA_ASSET_TYPE_TEXT,
        .handle = NYA_ASSETS_TEXTS_HELLO_TXT,
    });
    nya_asset_acquire(NYA_ASSETS_TEXTS_HELLO_TXT);
  }

  NYA_Asset* hello_txt_asset = nya_asset_get(NYA_ASSETS_TEXTS_HELLO_TXT);
  nya_asset_with(hello_txt_asset) {
    nya_info("hello.txt: %.*s", (s32)hello_txt_asset->as_text.size, (char*)hello_txt_asset->as_text.data);
  }

  NYA_Asset* vertex_shader_asset   = nya_asset_get(NYA_ASSETS_SHADERS_SOURCE_SAMPLE_VERT_HLSL);
  NYA_Asset* fragment_shader_asset = nya_asset_get(NYA_ASSETS_SHADERS_SOURCE_SAMPLE_FRAG_HLSL);
  nya_unused(vertex_shader_asset, fragment_shader_asset);
}

void on_event(NYA_Window* window, NYA_Event* event) {
  nya_unused(window, event);
}

void on_render(NYA_Window* window) {
  nya_unused(window);
}
