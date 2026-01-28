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
    .frame_allocator  = nya_arena_create(.name = "frame_allocator"),
  };

  nya_info("Nyangine initialized. Initializing subsystems...");

  nya_system_job_init();
  nya_system_render_init();
  nya_system_window_init();
  nya_system_events_init();
  nya_system_input_init();
  nya_system_asset_init();
  nya_system_entity_init();

  nya_info("Subsystems initialized successfully.");
}

void nya_app_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  nya_info("Deinitializing subsystems...");

  nya_system_entity_deinit();
  nya_system_asset_deinit();
  nya_system_input_deinit();
  nya_system_events_deinit();
  nya_system_window_deinit();
  nya_system_render_deinit();
  nya_system_job_deinit();

  nya_info("Subsystems deinitialized successfully. Good Bye.");

  nya_arena_destroy(app->global_allocator);
  nya_arena_destroy(app->frame_allocator);

  app->initialized = false;
}

void nya_app_options_update(NYA_AppConfig config) {
  NYA_App* app = nya_app_get_instance();

  nya_render_set_vsync(config.vsync_enabled);

  app->config = config;
}

void nya_app_run(void) {
  NYA_App* app = nya_app_get_instance();

  while (!app->should_quit_game_loop) {
    nya_perf_time_this_scope("frame");
    nya_event_dispatch((NYA_Event){
        .type = NYA_EVENT_FRAME_STARTED,
    });

    u64 current_time_ms    = SDL_GetTicks();
    u64 elapsed_ms         = current_time_ms - app->previous_time_ms;
    app->previous_time_ms  = current_time_ms;
    app->time_behind_ms   += (s64)elapsed_ms;

    // handle events
    {
      nya_perf_time_this_scope("event_handling");
      nya_event_dispatch((NYA_Event){
          .type = NYA_EVENT_HANDLING_STARTED,
      });

      nya_system_event_drain_sdl_events();

      NYA_Event event;
      while (nya_system_event_poll(&event)) {
        if (event.was_handled) continue;

        nya_system_window_handle_event(&event);
        if (event.was_handled) continue;

        nya_system_input_handle_event(&event);
        if (event.was_handled) continue;

        nya_array_foreach (app->window_system.windows, window) {
          nya_array_foreach_reverse (window->layer_stack, layer) {
            if (layer->enabled && layer->on_event != nullptr) {
              layer->on_event(window, &event);
              if (event.was_handled) break;
            }
          }
          if (event.was_handled) break;
        }
      }

      nya_event_dispatch((NYA_Event){
          .type = NYA_EVENT_HANDLING_ENDED,
      });
    }

    // updating
    {
      while (app->time_behind_ms >= app->config.time_step_ms) {
        nya_perf_time_this_scope("updating");
        nya_event_dispatch((NYA_Event){
            .type = NYA_EVENT_UPDATING_STARTED,
        });

        nya_array_foreach (app->window_system.windows, window) {
          nya_array_foreach (window->layer_stack, layer) {
            if (layer->enabled && layer->on_update != nullptr) { /**/
              layer->on_update(window, (f32)(app->config.time_step_ms) / 1000.0F);
            }
          }
        }

        app->time_behind_ms -= app->config.time_step_ms;
        nya_event_dispatch((NYA_Event){
            .type = NYA_EVENT_UPDATING_ENDED,
        });
      }
    }

    // rendering
    {
      nya_perf_time_this_scope("rendering");
      nya_event_dispatch((NYA_Event){
          .type = NYA_EVENT_RENDERING_STARTED,
      });

      nya_array_foreach (app->window_system.windows, window) {
        nya_render_begin(window);
        nya_array_foreach (window->layer_stack, layer) {
          if (layer->enabled && layer->on_render != nullptr) layer->on_render(window);
        }
        nya_render_end(window);
      }

      nya_event_dispatch((NYA_Event){
          .type = NYA_EVENT_RENDERING_ENDED,
      });
    }

    nya_event_dispatch((NYA_Event){
        .type = NYA_EVENT_FRAME_ENDED,
    });

    // framerate limiting
    if (!app->config.vsync_enabled && app->config.frame_rate_limit > 0) {
      u64 min_frame_time_ms = 1000 / app->config.frame_rate_limit;
      if (elapsed_ms < min_frame_time_ms) SDL_Delay((u32)(min_frame_time_ms - elapsed_ms));
    }
  }
}
