#include "SDL3/SDL_timer.h"

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
    .initialized                   = true,
    .config                        = config,
    .global_allocator              = nya_arena_create(.name = "global_allocator"),
    .frame_allocator               = nya_arena_create(.name = "frame_allocator"),
    .frame_stats.min_frame_time_ns = 1'000'000'000 / (u64)config.frame_rate_limit,
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

  app->config                        = config;
  app->frame_stats.min_frame_time_ns = 1'000'000'000 / (u64)config.frame_rate_limit;
}

void nya_app_run(void) {
  NYA_App* app = nya_app_get_instance();

  while (!app->should_quit_game_loop) {
    nya_perf_time_this_scope("frame");

    // start of frame tasks
    {
      nya_event_dispatch((NYA_Event){
          .type = NYA_EVENT_FRAME_STARTED,
      });

      app->frame_stats.frame_start_time_ns  = SDL_GetTicksNS();
      app->frame_stats.elapsed_ns           = app->frame_stats.frame_start_time_ns - app->frame_stats.prev_frame_time_ns;
      app->frame_stats.time_behind_ns      += (s64)app->frame_stats.elapsed_ns;
    }

    // handle events
    {
      nya_perf_time_this_scope("frame_event_handling");
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
      while (app->frame_stats.time_behind_ns >= (s64)app->config.time_step_ns) {
        nya_perf_time_this_scope("frame_updating");
        nya_event_dispatch((NYA_Event){
            .type = NYA_EVENT_UPDATING_STARTED,
        });

        nya_array_foreach (app->window_system.windows, window) {
          nya_array_foreach (window->layer_stack, layer) {
            if (layer->enabled && layer->on_update != nullptr) { /**/
              app->frame_stats.delta_time_s = (f32)nya_time_ns_to_s(app->config.time_step_ns);
              layer->on_update(window, app->frame_stats.delta_time_s);
            }
          }
        }

        app->frame_stats.time_behind_ns -= (s64)app->config.time_step_ns;
        nya_event_dispatch((NYA_Event){
            .type = NYA_EVENT_UPDATING_ENDED,
        });
      }
    }

    // rendering
    {
      nya_perf_time_this_scope("frame_rendering");
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

    // end of frame tasks
    {
      app->frame_stats.frame_end_time_ns  = SDL_GetTicksNS();
      app->frame_stats.prev_frame_time_ns = app->frame_stats.frame_start_time_ns;
      app->frame_stats.fps                = 1.0F / (f32)nya_time_ns_to_s(app->frame_stats.elapsed_ns);

      nya_arena_free_all(app->frame_allocator);

      nya_event_dispatch((NYA_Event){
          .type = NYA_EVENT_FRAME_ENDED,
      });
    }

    // framerate limiting
    if (!app->config.vsync_enabled && app->config.frame_rate_limit > 0) {
      if (app->frame_stats.elapsed_ns < app->frame_stats.min_frame_time_ns) { /**/
        SDL_DelayNS(app->frame_stats.min_frame_time_ns - app->frame_stats.elapsed_ns);
      }
    }
  }
}
