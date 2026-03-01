#include "SDL3/SDL_gpu.h"

#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_system_window_init(void) {
  NYA_App* app = nya_app_get();

  app->window_system = (NYA_WindowSystem){
    .allocator = nya_arena_create(.name = "window_system_allocator"),
  };

  app->window_system.windows = nya_array_create(app->window_system.allocator, NYA_Window);

  nya_info("Window system initialized.");
}

void nya_system_window_deinit(void) {
  NYA_App* app = nya_app_get();

  SDL_WaitForGPUIdle(app->render_system.gpu_device);
  nya_array_foreach_reverse (app->window_system.windows, window) nya_window_destroy(window->id);
  nya_array_destroy(app->window_system.windows);

  nya_arena_destroy(app->window_system.allocator);

  nya_info("Window system deinitialized.");
}

void nya_system_window_handle_event(NYA_Event* event) {
  nya_assert(event != nullptr);

  NYA_App* app = nya_app_get();

  if (event->type == NYA_EVENT_QUIT) app->should_quit = true;

  if (event->type == NYA_EVENT_WINDOW_CLOSE_REQUESTED) {
    nya_window_destroy(event->as_window_event.window_id);
    if (app->window_system.windows->length == 0) app->should_quit = true;
  }

  if (event->type == NYA_EVENT_WINDOW_RESIZED) {
    NYA_Window* window = nya_window_get(event->as_window_resized_event.window_id);
    window->width      = event->as_window_resized_event.width;
    window->height     = event->as_window_resized_event.height;
  }
}

/*
 * ─────────────────────────────────────────────────────────
 * WINDOW FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void* nya_window_create(void* id, NYA_ConstCString title, u32 initial_width, u32 initial_height, NYA_WindowFlags flags) {
  nya_assert(title != nullptr);
  nya_assert(initial_width > 0, "Initial width must be greater than 0.");
  nya_assert(initial_height > 0, "Initial height must be greater than 0.");

  static u64 id_counter = 0;

  NYA_App* app = nya_app_get();

  SDL_Window* sdl_window = SDL_CreateWindow(title, (s32)initial_width, (s32)initial_height, flags);
  nya_assert(sdl_window != nullptr, "SDL_CreateWindow() failed: %s", SDL_GetError());

  NYA_Window nya_window = {
    .id          = id != nullptr ? id : (void*)(++id_counter),
    .title       = title,
    .sdl_window  = sdl_window,
    .width       = initial_width,
    .height      = initial_height,
    .layer_stack = nya_array_create(app->window_system.allocator, NYA_Layer),
  };
  nya_system_renderer_for_window_init(&nya_window);

  nya_array_push_back(app->window_system.windows, nya_window);

  nya_info("Created window '%s' (id: %p).", title, nya_window.id);

  return nya_window.id;
}

void nya_window_destroy(void* window_id) {
  nya_assert(window_id != nullptr);

  NYA_App* app = nya_app_get();

  NYA_Window* window = nya_window_get(window_id);

  nya_array_foreach_reverse (window->layer_stack, layer) {
    NYA_LayerOnDestroyFn on_destroy_fn = nya_callback_get(layer->on_destroy);
    if (on_destroy_fn != nullptr) on_destroy_fn(window);
  }
  nya_array_destroy(window->layer_stack);

  nya_system_renderer_for_window_deinit(window);
  SDL_DestroyWindow(window->sdl_window);

  nya_info("Destroyed window '%s' (id: %p).", window->title, window->id);

  nya_array_for (app->window_system.windows, idx) {
    NYA_Window* w = &app->window_system.windows->items[idx];
    if (w->id == window_id) {
      nya_array_remove(app->window_system.windows, idx);
      return;
    }
  }
}

NYA_Window* nya_window_get(void* window_id) {
  nya_assert(window_id != nullptr);

  NYA_App* app = nya_app_get();

  nya_array_foreach (app->window_system.windows, window) {
    if (window->id == window_id) return window;
  }

  nya_panic("Cannot get window: window with id '%p' not found.", window_id);
  nya_unreachable();
}

f32 nya_window_get_display_scale(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  return SDL_GetWindowDisplayScale(window->sdl_window);
}

void nya_window_get_maximum_size(void* window_id, OUT u32* max_width, OUT u32* max_height) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  s32 w, h;
  SDL_GetWindowMaximumSize(window->sdl_window, &w, &h);
  if (max_width) *max_width = (u32)w;
  if (max_height) *max_height = (u32)h;
}

void nya_window_get_minimum_size(void* window_id, OUT u32* min_width, OUT u32* min_height) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  s32 w, h;
  SDL_GetWindowMinimumSize(window->sdl_window, &w, &h);
  if (min_width) *min_width = (u32)w;
  if (min_height) *min_height = (u32)h;
}

void nya_window_get_position(void* window_id, OUT s32* x, OUT s32* y) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_GetWindowPosition(window->sdl_window, x, y);
}

void nya_window_get_size(void* window_id, OUT u32* width, OUT u32* height) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  if (width) *width = window->width;
  if (height) *height = window->height;
}

b8 nya_window_has_focus(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  return SDL_GetWindowFlags(window->sdl_window) & SDL_WINDOW_INPUT_FOCUS;
}

void nya_window_hide(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_HideWindow(window->sdl_window);
}

b8 nya_window_is_fullscreen(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  return SDL_GetWindowFlags(window->sdl_window) & SDL_WINDOW_FULLSCREEN;
}

b8 nya_window_is_maximized(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  return SDL_GetWindowFlags(window->sdl_window) & SDL_WINDOW_MAXIMIZED;
}

b8 nya_window_is_minimized(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  return SDL_GetWindowFlags(window->sdl_window) & SDL_WINDOW_MINIMIZED;
}

b8 nya_window_is_visible(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  return !(SDL_GetWindowFlags(window->sdl_window) & SDL_WINDOW_HIDDEN);
}

void nya_window_maximize(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_MaximizeWindow(window->sdl_window);
}

void nya_window_minimize(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_MinimizeWindow(window->sdl_window);
}

void nya_window_raise(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_RaiseWindow(window->sdl_window);
}

void nya_window_restore(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_RestoreWindow(window->sdl_window);
}

void nya_window_set_always_on_top(void* window_id, b8 always_on_top) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_SetWindowAlwaysOnTop(window->sdl_window, always_on_top);
}

void nya_window_set_borderless(void* window_id, b8 borderless) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_SetWindowBordered(window->sdl_window, !borderless);
}

void nya_window_set_fullscreen(void* window_id, b8 fullscreen) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_SetWindowFullscreen(window->sdl_window, fullscreen);
}

void nya_window_set_maximum_size(void* window_id, u32 max_width, u32 max_height) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_SetWindowMaximumSize(window->sdl_window, (s32)max_width, (s32)max_height);
}

void nya_window_set_minimum_size(void* window_id, u32 min_width, u32 min_height) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_SetWindowMinimumSize(window->sdl_window, (s32)min_width, (s32)min_height);
}

void nya_window_set_position(void* window_id, s32 x, s32 y) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_SetWindowPosition(window->sdl_window, x, y);
}

void nya_window_set_resizable(void* window_id, b8 resizable) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_SetWindowResizable(window->sdl_window, resizable);
}

void nya_window_set_size(void* window_id, u32 width, u32 height) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_SetWindowSize(window->sdl_window, (s32)width, (s32)height);
  window->width  = width;
  window->height = height;
}

void nya_window_set_title(void* window_id, NYA_ConstCString title) {
  nya_assert(window_id != nullptr);
  nya_assert(title != nullptr);

  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);

  SDL_SetWindowTitle(window->sdl_window, title);
}

void nya_window_show(void* window_id) {
  nya_assert(window_id != nullptr);
  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window != nullptr);
  SDL_ShowWindow(window->sdl_window);
}

/*
 * ─────────────────────────────────────────────────────────
 * LAYER FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_Layer* nya_layer_get(void* window_id, void* layer_id) {
  nya_assert(window_id != nullptr);
  nya_assert(layer_id != nullptr);

  NYA_Window* window = nya_window_get(window_id);
  nya_array_foreach (window->layer_stack, layer) {
    if (layer->id == layer_id) return layer;
  }

  nya_panic("Cannot get layer: layer with id '%p' not found.", layer_id);
  nya_unreachable();
}

void nya_layer_enable(void* window_id, void* layer_id) {
  nya_assert(window_id != nullptr);
  nya_assert(layer_id != nullptr);

  NYA_Layer* layer = nya_layer_get(window_id, layer_id);
  layer->enabled   = true;
}

void nya_layer_disable(void* window_id, void* layer_id) {
  nya_assert(window_id != nullptr);
  nya_assert(layer_id != nullptr);

  NYA_Layer* layer = nya_layer_get(window_id, layer_id);
  layer->enabled   = false;
}

void nya_layer_push(void* window_id, NYA_Layer layer) {
  nya_assert(window_id != nullptr);

  NYA_Window* window = nya_window_get(window_id);

  NYA_LayerOnCreateFn on_create_fn = nya_callback_get(layer.on_create);
  if (on_create_fn != nullptr) on_create_fn(window);

  nya_array_push_back(window->layer_stack, layer);
}

NYA_Layer nya_layer_pop(void* window_id) {
  nya_assert(window_id != nullptr);

  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window->layer_stack->length > 0, "Cannot pop layer: layer stack is empty.");

  NYA_Layer*           layer         = nya_array_last(window->layer_stack);
  NYA_LayerOnDestroyFn on_destroy_fn = nya_callback_get(layer->on_destroy);
  if (on_destroy_fn != nullptr) on_destroy_fn(window);

  return nya_array_pop_back(window->layer_stack);
}
