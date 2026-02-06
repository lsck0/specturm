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
  NYA_App* app = nya_app_get_instance();

  app->window_system = (NYA_WindowSystem){
    .allocator = nya_arena_create(.name = "window_system_allocator"),
  };

  app->window_system.windows = nya_array_create(app->window_system.allocator, NYA_Window);

  nya_info("Window system initialized.");
}

void nya_system_window_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  SDL_WaitForGPUIdle(app->render_system.gpu_device);
  nya_array_foreach_reverse (app->window_system.windows, window) nya_window_destroy(window->id);
  nya_array_destroy(app->window_system.windows);

  nya_arena_destroy(app->window_system.allocator);

  nya_info("Window system deinitialized.");
}

void nya_system_window_handle_event(NYA_Event* event) {
  nya_assert(event != nullptr);

  NYA_App* app = nya_app_get_instance();

  if (event->type == NYA_EVENT_QUIT) app->should_quit_game_loop = true;

  if (event->type == NYA_EVENT_WINDOW_CLOSE_REQUESTED) {
    nya_window_destroy(event->as_window_event.window_id);
    if (app->window_system.windows->length == 0) app->should_quit_game_loop = true;
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

void* nya_window_create(NYA_ConstCString title, u32 initial_width, u32 initial_height, NYA_WindowFlags flags, void* id) {
  nya_assert(title != nullptr);
  nya_assert(initial_width > 0, "Initial width must be greater than 0.");
  nya_assert(initial_height > 0, "Initial height must be greater than 0.");

  static u64 id_counter = 0;

  NYA_App* app = nya_app_get_instance();

  SDL_Window* sdl_window = SDL_CreateWindow(title, (s32)initial_width, (s32)initial_height, flags);
  nya_assert(sdl_window != nullptr, "SDL_CreateWindow() failed: %s", SDL_GetError());

  NYA_Window nya_window = {
    .id          = id != nullptr ? id : (void*)(++id_counter),
    .title       = title,
    .sdl_window  = sdl_window,
    .width       = initial_width,
    .height      = initial_height,
    .layer_stack = nya_array_create(app->global_allocator, NYA_Layer),
  };
  nya_system_render_for_window_init(&nya_window);

  nya_array_push_back(app->window_system.windows, nya_window);

  nya_info("Created window '%s' (id: %p).", title, nya_window.id);

  return nya_window.id;
}

void nya_window_destroy(void* window_id) {
  nya_assert(window_id != nullptr);

  NYA_App* app = nya_app_get_instance();

  NYA_Window* window = nya_window_get(window_id);

  nya_array_foreach_reverse (window->layer_stack, layer) {
    NYA_LayerOnDestroyFn on_destroy_fn = nya_callback_get(layer->on_create);
    if (on_destroy_fn != nullptr) on_destroy_fn();
  }
  nya_array_destroy(window->layer_stack);

  nya_system_render_for_window_deinit(window);
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

  NYA_App* app = nya_app_get_instance();

  nya_array_foreach (app->window_system.windows, window) {
    if (window->id == window_id) return window;
  }

  nya_panic("Cannot get window: window with id '%p' not found.", window_id);
  nya_unreachable();
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
  if (on_create_fn != nullptr) on_create_fn();

  nya_array_push_back(window->layer_stack, layer);
}

NYA_Layer nya_layer_pop(void* window_id) {
  nya_assert(window_id != nullptr);

  NYA_Window* window = nya_window_get(window_id);
  nya_assert(window->layer_stack->length > 0, "Cannot pop layer: layer stack is empty.");

  NYA_Layer            layer         = nya_array_last(window->layer_stack);
  NYA_LayerOnDestroyFn on_destroy_fn = nya_callback_get(layer.on_destroy);
  if (on_destroy_fn != nullptr) on_destroy_fn();

  return nya_array_pop_back(window->layer_stack);
}
