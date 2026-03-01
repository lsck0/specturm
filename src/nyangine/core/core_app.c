#include "SDL3/SDL_init.h"
#include "SDL3/SDL_timer.h"

#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL NYA_App _NYA_APP_INSTANCE;

NYA_INTERNAL void _nya_app_handle_shutdown_signal(NYA_Signal signal);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void nya_app_init_with_options(NYA_AppOptions options) {
  nya_assert(options.time_step_ns > 0, "time_step_ns must be greater than 0");
  nya_assert(options.frame_rate_limit > 0, "frame_rate_limit must be greater than 0");
  nya_assert(options.max_concurrent_jobs > 0, "max_concurrent_jobs must be greater than 0");

  nya_integrity_assert();

  nya_signals_init();
  nya_signals_set_handler(NYA_SIGNAL_HANGUP, _nya_app_handle_shutdown_signal);
  nya_signals_set_handler(NYA_SIGNAL_INTERRUPT, _nya_app_handle_shutdown_signal);
  nya_signals_set_handler(NYA_SIGNAL_TERMINATE, _nya_app_handle_shutdown_signal);

  b8 ok = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  nya_assert(ok, "SDL_Init() failed: %s", SDL_GetError());

  NYA_App* app = &_NYA_APP_INSTANCE;
  *app         = (NYA_App){
            .initialized                    = true,
            .options                        = options,
            .frame_allocator                = nya_arena_create(.name = "frame_allocator"),
            .frame_stats.started_ns         = nya_clock_get_timestamp_ns(),
            .frame_stats.prev_frame_time_ns = nya_clock_get_timestamp_ns(),
            .frame_stats.min_frame_time_ns  = 1'000'000'000 / (u64)options.frame_rate_limit,
  };

  nya_info("Nyangine initialized. Initializing subsystems...");

  nya_system_job_init();
  nya_system_callback_init();
  nya_system_renderer_init();
  nya_system_window_init();
  nya_system_events_init();
  nya_system_input_init();
  nya_system_asset_init();
  nya_system_entity_init();

  nya_info("Subsystems initialized successfully.");
}

void nya_app_deinit(void) {
  NYA_App* app = nya_app_get();

  nya_info("Deinitializing subsystems...");

  nya_system_entity_deinit();
  nya_system_asset_deinit();
  nya_system_input_deinit();
  nya_system_events_deinit();
  nya_system_window_deinit();
  nya_system_renderer_deinit();
  nya_system_job_deinit();
  nya_system_callback_deinit();

  nya_info("Subsystems deinitialized successfully.");

  nya_arena_destroy(app->frame_allocator);

  SDL_Quit();

  nya_signals_deinit();

  app->initialized = false;

  nya_info("Nyangine deinitialized. Goodbye.");
}

void nya_app_run(void) {
  NYA_App* app = nya_app_get();

  while (!app->should_quit) {
    nya_perf_time_this_scope("frame");

    // start of frame tasks
    {
      nya_event_dispatch((NYA_Event){
          .type = NYA_EVENT_FRAME_STARTED,
      });

      app->frame_stats.now_ns               = nya_clock_get_timestamp_ns() - app->frame_stats.started_ns;
      app->frame_stats.frame_start_time_ns  = nya_clock_get_timestamp_ns();
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
            NYA_LayerOnEventFn on_event_fn = nya_callback_get(layer->on_event);
            if (layer->enabled && on_event_fn != nullptr) {
              on_event_fn(window, &event);
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
      while (app->frame_stats.time_behind_ns >= (s64)app->options.time_step_ns) {
        nya_perf_time_this_scope("frame_updating");
        nya_event_dispatch((NYA_Event){
            .type = NYA_EVENT_UPDATING_STARTED,
        });

        nya_array_foreach (app->window_system.windows, window) {
          nya_array_foreach (window->layer_stack, layer) {
            NYA_LayerOnUpdateFn on_update_fn = nya_callback_get(layer->on_update);
            if (layer->enabled && on_update_fn != nullptr) { /**/
              app->frame_stats.delta_time_s = (f32)nya_time_ns_to_s(app->options.time_step_ns);
              on_update_fn(window, app->frame_stats.delta_time_s);
            }
          }
        }

        app->frame_stats.time_behind_ns -= (s64)app->options.time_step_ns;
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
        if (window->render_system.render_pass == nullptr) continue;

        nya_array_foreach (window->layer_stack, layer) {
          NYA_LayerOnRenderFn on_render_fn = nya_callback_get(layer->on_render);
          if (layer->enabled && on_render_fn != nullptr) on_render_fn(window);
        }

        nya_render_end(window);
      }

      nya_event_dispatch((NYA_Event){
          .type = NYA_EVENT_RENDERING_ENDED,
      });
    }

    // end of frame tasks
    {
      app->frame_stats.frame_end_time_ns  = nya_clock_get_timestamp_ns();
      app->frame_stats.prev_frame_time_ns = app->frame_stats.frame_start_time_ns;
      app->frame_stats.fps                = 1.0F / (f32)nya_time_ns_to_s(app->frame_stats.elapsed_ns);

      nya_arena_free_all(app->frame_allocator);

      nya_event_dispatch((NYA_Event){
          .type = NYA_EVENT_FRAME_ENDED,
      });
    }

    // framerate limiting
    if (!app->options.vsync_enabled && app->options.frame_rate_limit > 0) {
      if (app->frame_stats.elapsed_ns < app->frame_stats.min_frame_time_ns) { /**/
        SDL_DelayNS(app->frame_stats.min_frame_time_ns - app->frame_stats.elapsed_ns);
      }
    }
  }
}

NYA_App* nya_app_get(void) {
  nya_assert(_NYA_APP_INSTANCE.initialized);
  return &_NYA_APP_INSTANCE;
}

void nya_app_options_update(NYA_AppOptions options) {
  nya_assert(options.time_step_ns > 0, "time_step_ns must be greater than 0");
  nya_assert(options.frame_rate_limit > 0, "frame_rate_limit must be greater than 0");
  nya_assert(options.max_concurrent_jobs > 0, "max_concurrent_jobs must be greater than 0");

  NYA_App* app = nya_app_get();

  nya_system_renderer_set_vsync(options.vsync_enabled);

  app->options                       = options;
  app->frame_stats.min_frame_time_ns = 1'000'000'000 / (u64)options.frame_rate_limit;
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL void _nya_app_handle_shutdown_signal(NYA_Signal signal) {
  nya_unused(signal);

  NYA_App* app     = nya_app_get();
  app->should_quit = true;
}
