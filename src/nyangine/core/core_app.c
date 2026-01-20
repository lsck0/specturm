#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_timer.h"

#include "nyangine/core/core_event.h"
#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL NYA_App NYA_APP_INSTANCE;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_App* nya_app_get_instance(void) {
  nya_assert(NYA_APP_INSTANCE.initialized);
  return &NYA_APP_INSTANCE;
}

void nya_app_init(NYA_AppConfig config) {
  NYA_App* app = &NYA_APP_INSTANCE;

  b8 ok = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  nya_assert(ok, "SDL_Init() failed: %s", SDL_GetError());

  *app = (NYA_App){
      .initialized      = true,
      .config           = config,
      .global_allocator = nya_arena_create(.name = "global_allocator"),
      .entity_allocator = nya_arena_create(.name = "entity_allocator", .defragmentation_enabled = false),
      .frame_allocator  = nya_arena_create(.name = "frame_allocator"),
  };

  nya_system_render_init();
  nya_system_window_init();
  nya_system_events_init();
  nya_system_input_init();
  nya_system_asset_init();
  nya_system_entity_init();
}

void nya_app_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  nya_system_entity_deinit();
  nya_system_asset_deinit();
  nya_system_input_deinit();
  nya_system_events_deinit();
  nya_system_window_deinit();
  nya_system_render_deinit();

  nya_arena_destroy(&app->global_allocator);
  nya_arena_destroy(&app->entity_allocator);
  nya_arena_destroy(&app->frame_allocator);

  app->initialized = false;
}

void nya_app_options_update(NYA_AppConfig config) {
  NYA_App* app = nya_app_get_instance();

  // move into the renderer system?
  if (app->config.vsync_enabled != config.vsync_enabled) {
    nya_array_foreach (&app->window_system.windows, window) {
      b8 ok = SDL_SetGPUSwapchainParameters(
          app->render_system.gpu_device,
          window->sdl_window,
          SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
          config.vsync_enabled ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_MAILBOX
      );
      nya_assert(ok, "SDL_SetGPUSwapchainParameters() failed: %s", SDL_GetError());
    }
  }

  app->config = config;
}

void nya_app_run(void) {
  NYA_App* app = nya_app_get_instance();

  while (!app->should_quit_game_loop) {
    u64 current_time_ms    = SDL_GetTicks();
    u64 elapsed_ms         = current_time_ms - app->previous_time_ms;
    app->previous_time_ms  = current_time_ms;
    app->time_behind_ms   += (s64)elapsed_ms;

    // handle events
    {
      nya_event_drain_sdl_events();

      NYA_Event event;
      while (nya_event_poll(&event)) {
        if (event.was_handled) continue;

        if (event.type == NYA_EVENT_QUIT) app->should_quit_game_loop = true;
        if (event.type == NYA_EVENT_WINDOW_CLOSE_REQUESTED) {
          nya_window_destroy(event.as_window_event.window_id);
          if (app->window_system.windows.length == 0) app->should_quit_game_loop = true;
        }
        if (event.type == NYA_EVENT_WINDOW_RESIZED) {
          NYA_Window* window = nya_window_get(event.as_window_resized_event.window_id);
          window->width      = event.as_window_resized_event.width;
          window->height     = event.as_window_resized_event.height;
        }

        nya_array_foreach (&app->window_system.windows, window) {
          nya_array_foreach_reverse (&window->layer_stack, layer) {
            if (layer->enabled && layer->on_event != nullptr) {
              layer->on_event(window, &event);
              if (event.was_handled) break;
            }
          }
          if (event.was_handled) break;
        }
      }
    }

    // updating
    {
      while (app->time_behind_ms >= app->config.time_step_ms) {
        nya_array_foreach (&app->window_system.windows, window) {
          nya_array_foreach (&window->layer_stack, layer) {
            if (layer->enabled && layer->on_update != nullptr) {
              layer->on_update(window, (f32)(app->config.time_step_ms) / 1000.0F);
            }
          }
        }
        app->time_behind_ms -= app->config.time_step_ms;
      }
    }

    // rendering
    {
      nya_array_foreach (&app->window_system.windows, window) {
        nya_render_begin(window);
        nya_array_foreach (&window->layer_stack, layer) {
          if (layer->enabled && layer->on_render != nullptr) layer->on_render(window);
        }
        nya_render_end(window);
      }
    }

    // framerate limiting
    if (!app->config.vsync_enabled && app->config.frame_rate_limit > 0) {
      u64 min_frame_time_ms = 1000 / app->config.frame_rate_limit;
      if (elapsed_ms < min_frame_time_ms) SDL_Delay((u32)(min_frame_time_ms - elapsed_ms));
    }
  }
}
