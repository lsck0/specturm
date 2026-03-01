#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_string.h"
#include "nyangine/core/core_asset.h"
#include "nyangine/core/core_callback.h"
#include "nyangine/core/core_entity.h"
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
typedef struct NYA_AppOptions NYA_AppOptions;
typedef struct NYA_FrameStats NYA_FrameStats;

#define _NYA_APP_DEFAULT_OPTIONS .time_step_ns = nya_time_ms_to_ns(16), .frame_rate_limit = 120, .vsync_enabled = false, .max_concurrent_jobs = 4

struct NYA_AppOptions {
  u64 time_step_ns;
  u32 frame_rate_limit;
  b8  vsync_enabled;
  u8  max_concurrent_jobs;
};

struct NYA_FrameStats {
  u64 started_ns;
  u64 now_ns;

  u64 min_frame_time_ns;
  f32 delta_time_s;
  f32 fps;

  u64 frame_start_time_ns;
  u64 frame_end_time_ns;
  u64 prev_frame_time_ns;
  u64 elapsed_ns;
  s64 time_behind_ns;
};

struct NYA_App {
  b8 initialized;
  b8 should_quit;

  /** use `nya_app_options_update` to change config */
  NYA_AppOptions options;

  NYA_Arena* frame_allocator;

  NYA_FrameStats frame_stats;

  NYA_AssetSystem    asset_system;
  NYA_CallbackSystem callback_system;
  NYA_EntitySystem   entity_system;
  NYA_EventSystem    event_system;
  NYA_InputSystem    input_system;
  NYA_JobSystem      job_system;
  NYA_RenderSystem   render_system;
  NYA_WindowSystem   window_system;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS AND MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_app_init(...) nya_app_init_with_options((NYA_AppOptions){ _NYA_APP_DEFAULT_OPTIONS, __VA_ARGS__ })
NYA_API NYA_EXTERN void     nya_app_init_with_options(NYA_AppOptions options);
NYA_API NYA_EXTERN void     nya_app_deinit(void);
NYA_API NYA_EXTERN void     nya_app_run(void);
NYA_API NYA_EXTERN NYA_App* nya_app_get(void);
NYA_API NYA_EXTERN void     nya_app_options_update(NYA_AppOptions options);
