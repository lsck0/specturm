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
  NYA_WINDOW_NONE               = 0,
  NYA_WINDOW_ALWAYS_ON_TOP      = SDL_WINDOW_ALWAYS_ON_TOP,
  NYA_WINDOW_BORDERLESS         = SDL_WINDOW_BORDERLESS,
  NYA_WINDOW_FULLSCREEN         = SDL_WINDOW_FULLSCREEN,
  NYA_WINDOW_HIDDEN             = SDL_WINDOW_HIDDEN,
  NYA_WINDOW_HIGH_PIXEL_DENSITY = SDL_WINDOW_HIGH_PIXEL_DENSITY,
  NYA_WINDOW_MAXIMIZED          = SDL_WINDOW_MAXIMIZED,
  NYA_WINDOW_MINIMIZED          = SDL_WINDOW_MINIMIZED,
  NYA_WINDOW_MODAL              = SDL_WINDOW_MODAL,
  NYA_WINDOW_NOT_FOCUSABLE      = SDL_WINDOW_NOT_FOCUSABLE,
  NYA_WINDOW_POPUP_MENU         = SDL_WINDOW_POPUP_MENU,
  NYA_WINDOW_RESIZABLE          = SDL_WINDOW_RESIZABLE,
  NYA_WINDOW_TRANSPARENT        = SDL_WINDOW_TRANSPARENT,
  NYA_WINDOW_UTILITY            = SDL_WINDOW_UTILITY,
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
  NYA_CallbackHandle on_event;
  NYA_CallbackHandle on_update;
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

NYA_API NYA_EXTERN b8   nya_window_has_focus(void* window_id);
NYA_API NYA_EXTERN b8   nya_window_is_fullscreen(void* window_id);
NYA_API NYA_EXTERN b8   nya_window_is_maximized(void* window_id);
NYA_API NYA_EXTERN b8   nya_window_is_minimized(void* window_id);
NYA_API NYA_EXTERN b8   nya_window_is_visible(void* window_id);
NYA_API NYA_EXTERN f32  nya_window_get_display_scale(void* window_id);
NYA_API NYA_EXTERN void nya_window_get_maximum_size(void* window_id, OUT u32* max_width, OUT u32* max_height);
NYA_API NYA_EXTERN void nya_window_get_minimum_size(void* window_id, OUT u32* min_width, OUT u32* min_height);
NYA_API NYA_EXTERN void nya_window_get_position(void* window_id, OUT s32* x, OUT s32* y);
NYA_API NYA_EXTERN void nya_window_get_size(void* window_id, OUT u32* width, OUT u32* height);
NYA_API NYA_EXTERN void nya_window_hide(void* window_id);
NYA_API NYA_EXTERN void nya_window_maximize(void* window_id);
NYA_API NYA_EXTERN void nya_window_minimize(void* window_id);
NYA_API NYA_EXTERN void nya_window_raise(void* window_id);
NYA_API NYA_EXTERN void nya_window_restore(void* window_id);
NYA_API NYA_EXTERN void nya_window_set_always_on_top(void* window_id, b8 always_on_top);
NYA_API NYA_EXTERN void nya_window_set_borderless(void* window_id, b8 borderless);
NYA_API NYA_EXTERN void nya_window_set_fullscreen(void* window_id, b8 fullscreen);
NYA_API NYA_EXTERN void nya_window_set_maximum_size(void* window_id, u32 max_width, u32 max_height);
NYA_API NYA_EXTERN void nya_window_set_minimum_size(void* window_id, u32 min_width, u32 min_height);
NYA_API NYA_EXTERN void nya_window_set_position(void* window_id, s32 x, s32 y);
NYA_API NYA_EXTERN void nya_window_set_resizable(void* window_id, b8 resizable);
NYA_API NYA_EXTERN void nya_window_set_size(void* window_id, u32 width, u32 height);
NYA_API NYA_EXTERN void nya_window_set_title(void* window_id, NYA_ConstCString title);
NYA_API NYA_EXTERN void nya_window_show(void* window_id);

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
