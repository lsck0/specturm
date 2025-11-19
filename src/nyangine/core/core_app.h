#pragma once

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_string.h"
#include "nyangine/core/core_event.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPE DEFINITIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_Layer     NYA_Layer;
typedef struct NYA_Window    NYA_Window;
typedef struct NYA_App       NYA_App;
typedef struct NYA_AppConfig NYA_AppConfig;
nya_derive_array(NYA_Layer);
nya_derive_array(NYA_Window);

struct NYA_AppConfig {
  u32 time_step_ms;
  u32 frame_rate_limit;
  b8  vsync_enabled;
};

struct NYA_App {
  /** use `nya_app_options_update` to change config */
  NYA_AppConfig config;

  NYA_Arena global_allocator;
  NYA_Arena entity_allocator;
  NYA_Arena frame_allocator;

  SDL_GPUDevice*  gpu_device;
  NYA_WindowArray windows;

  u64 previous_time_ms;
  s64 time_behind_ms;
  b8  should_quit_game_loop;
};

typedef enum {
  NYA_WINDOW_NONE      = 0,
  NYA_WINDOW_RESIZABLE = SDL_WINDOW_RESIZABLE,
} NYA_WindowFlags;

struct NYA_Window {
  void*       id;
  SDL_Window* sdl_window;

  NYA_LayerArray layer_stack;
};

struct NYA_Layer {
  void* id;
  b8    enabled;

  void (*on_create)(void);
  void (*on_destroy)(void);

  void (*on_update)(NYA_Window* window, f32 delta_time);
  void (*on_event)(NYA_Window* window, NYA_Event* event);
  void (*on_render)(NYA_Window* window);
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * APP FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN NYA_App nya_app_new(NYA_AppConfig config);
NYA_API NYA_EXTERN void    nya_app_destroy(NYA_App* app);
NYA_API NYA_EXTERN void    nya_app_options_update(NYA_App* app, NYA_AppConfig config);
NYA_API NYA_EXTERN void    nya_app_run(NYA_App* app);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * WINDOW FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

// clang-format off
NYA_API NYA_EXTERN void*       nya_window_new(NYA_App* app, const char* title, u32 initial_width, u32 initial_height, NYA_WindowFlags flags, void* id);
NYA_API NYA_EXTERN void        nya_window_destroy(NYA_App* app, void* window_id);
NYA_API NYA_EXTERN NYA_Window* nya_window_get(NYA_App* app, void* window_id);
NYA_API NYA_EXTERN NYA_Layer*  nya_window_layer_get(NYA_App* app, void* window_id, void* layer_id);
NYA_API NYA_EXTERN void        nya_window_layer_enable(NYA_App* app, void* window_id, void* layer_id);
NYA_API NYA_EXTERN void        nya_window_layer_disable(NYA_App* app, void* window_id, void* layer_id);
NYA_API NYA_EXTERN void        nya_window_layer_push(NYA_App* app, void* window_id, NYA_Layer layer);
NYA_API NYA_EXTERN NYA_Layer   nya_window_layer_pop(NYA_App* app, void* window_id);
// clang-format on
