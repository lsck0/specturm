#include "gnyame/gnyame.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ON CREATE
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gny_layer_game_on_create(void) {
  nya_asset_load((NYA_AssetLoadParameters){
      .type   = NYA_ASSET_TYPE_SHADER_VERTEX,
      .handle = NYA_ASSETS_SHADERS_SOURCE_SAMPLE_VERT_HLSL,
  });

  nya_asset_load((NYA_AssetLoadParameters){
      .type   = NYA_ASSET_TYPE_SHADER_FRAGMENT,
      .handle = NYA_ASSETS_SHADERS_SOURCE_SAMPLE_FRAG_HLSL,
  });
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ON DESTROY
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gny_layer_game_on_destroy(void) {
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ON EVENT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gny_layer_game_on_event(NYA_Window* window, NYA_Event* event) {
  nya_unused(window, event);
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ON UPDATE
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gny_layer_game_on_update(NYA_Window* window, f32 delta_time_s) {
  nya_unused(window, delta_time_s);

  if (nya_input_key_just_pressed(NYA_KEY_A)) { nya_asset_release(NYA_ASSETS_TEXTS_HELLO_TXT); }

  if (nya_input_key_just_pressed(NYA_KEY_B)) {
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

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ON RENDER
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void gny_layer_game_on_render(NYA_Window* window) {
  nya_unused(window);
}
