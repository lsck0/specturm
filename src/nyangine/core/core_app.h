#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_string.h"
#include "nyangine/core/core_asset.h"
#include "nyangine/core/core_event.h"
#include "nyangine/core/core_input.h"
#include "nyangine/core/core_window.h"
#include "nyangine/renderer/renderer.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_App       NYA_App;
typedef struct NYA_AppConfig NYA_AppConfig;

struct NYA_AppConfig {
  u32 time_step_ms;
  u32 frame_rate_limit;
  b8  vsync_enabled;
};

struct NYA_App {
  b8 initialized;

  /** use `nya_app_options_update` to change config */
  NYA_AppConfig config;

  NYA_Arena global_allocator;
  NYA_Arena entity_allocator;
  NYA_Arena frame_allocator;

  NYA_WindowSystem window_system;
  NYA_EventSystem  event_system;
  NYA_InputSystem  input_system;
  NYA_AssetSystem  asset_system;
  NYA_RenderSystem render_system;

  u64 previous_time_ms;
  s64 time_behind_ms;
  b8  should_quit_game_loop;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void     nya_app_init(NYA_AppConfig config);
NYA_API NYA_EXTERN void     nya_app_deinit(void);
NYA_API NYA_EXTERN NYA_App* nya_app_get_instance(void);
NYA_API NYA_EXTERN void     nya_app_options_update(NYA_AppConfig config);
NYA_API NYA_EXTERN void     nya_app_run(void);
