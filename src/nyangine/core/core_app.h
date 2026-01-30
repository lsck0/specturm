#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_string.h"
#include "nyangine/core/core_asset.h"
#include "nyangine/core/core_event.h"
#include "nyangine/core/core_input.h"
#include "nyangine/core/core_job.h"
#include "nyangine/core/core_window.h"
#include "nyangine/renderer/renderer.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_App        NYA_App;
typedef struct NYA_AppConfig  NYA_AppConfig;
typedef struct NYA_FrameStats NYA_FrameStats;

struct NYA_AppConfig {
  u32 time_step_ns;
  u32 frame_rate_limit;
  b8  vsync_enabled;
};

struct NYA_FrameStats {
  u64 min_frame_time_ns;
  u64 frame_start_time_ns;
  u64 frame_end_time_ns;
  u64 elapsed_ns;
  s64 time_behind_ns;
  f32 delta_time_s;
  f32 fps;
};

struct NYA_App {
  b8 initialized;

  /** use `nya_app_options_update` to change config */
  NYA_AppConfig config;

  NYA_Arena* global_allocator;
  NYA_Arena* frame_allocator;

  NYA_AssetSystem  asset_system;
  NYA_EventSystem  event_system;
  NYA_InputSystem  input_system;
  NYA_JobSystem    job_system;
  NYA_RenderSystem render_system;
  NYA_WindowSystem window_system;

  NYA_FrameStats frame_stats;

  b8 should_quit_game_loop;
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
