#pragma once

#include "SDL3/SDL_gpu.h"

#include "nyangine/base/base_array.h"
#include "nyangine/base/base_string.h"
#include "nyangine/core/core_callback.h"
#include "nyangine/core/core_event.h"
#include "nyangine/renderer/renderer.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef enum NYA_WindowFlags    NYA_WindowFlags;
typedef struct NYA_Layer        NYA_Layer;
typedef struct NYA_Window       NYA_Window;
typedef struct NYA_WindowSystem NYA_WindowSystem;
nya_derive_array(NYA_Layer);
nya_derive_array(NYA_Window);

typedef void (*NYA_LayerOnCreateFn)(void);
typedef void (*NYA_LayerOnDestroyFn)(void);
typedef void (*NYA_LayerOnUpdateFn)(NYA_Window* window, f32 delta_time_s);
typedef void (*NYA_LayerOnEventFn)(NYA_Window* window, NYA_Event* event);
typedef void (*NYA_LayerOnRenderFn)(NYA_Window* window);

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_WindowSystem {
  NYA_Arena* allocator;

  NYA_WindowArray* windows;
};

/*
 * ─────────────────────────────────────────────────────────
 * WINDOW STRUCT
 * ─────────────────────────────────────────────────────────
 */

enum NYA_WindowFlags {
  NYA_WINDOW_NONE      = 0,
  NYA_WINDOW_RESIZABLE = SDL_WINDOW_RESIZABLE,
};

struct NYA_Window {
  void*            id;
  NYA_ConstCString title;
  SDL_Window*      sdl_window;

  // controlled by NYA_WINDOW_RESIZE events
  u32 width;
  u32 height;

  // this is the swapchain texture size
  u32 screen_width;
  u32 screen_height;

  NYA_LayerArray* layer_stack;

  NYA_RenderSystemWindow render_system;
};

/*
 * ─────────────────────────────────────────────────────────
 * LAYER STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_Layer {
  void* id;
  b8    enabled;

  NYA_CallbackHandle on_create;
  NYA_CallbackHandle on_destroy;
  NYA_CallbackHandle on_update;
  NYA_CallbackHandle on_event;
  NYA_CallbackHandle on_render;

  /** Set by nya_layer_push. */

  void* window_id;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_system_window_init(void);
NYA_API NYA_EXTERN void nya_system_window_deinit(void);
NYA_API NYA_EXTERN void nya_system_window_handle_event(NYA_Event* event);

/*
 * ─────────────────────────────────────────────────────────
 * WINDOW FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void*       nya_window_create(NYA_ConstCString title, u32 initial_width, u32 initial_height, NYA_WindowFlags flags, void* id);
NYA_API NYA_EXTERN void        nya_window_destroy(void* window_id);
NYA_API NYA_EXTERN NYA_Window* nya_window_get(void* window_id);

/*
 * ─────────────────────────────────────────────────────────
 * LAYER FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN NYA_Layer* nya_layer_get(void* window_id, void* layer_id);
NYA_API NYA_EXTERN void       nya_layer_enable(void* window_id, void* layer_id);
NYA_API NYA_EXTERN void       nya_layer_disable(void* window_id, void* layer_id);
NYA_API NYA_EXTERN void       nya_layer_push(void* window_id, NYA_Layer layer);
NYA_API NYA_EXTERN NYA_Layer  nya_layer_pop(void* window_id);
