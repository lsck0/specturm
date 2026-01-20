#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL NYA_Event _nya_event_from_sdl_event(SDL_Event sdl_event);
NYA_INTERNAL void*     _nya_event_sdl_window_id_to_nya_window_id(SDL_WindowID sdl_window_id);
NYA_INTERNAL void      _nya_event_notify_listeners(NYA_Event* event);

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

void nya_system_events_init(void) {
  NYA_App* app = nya_app_get_instance();

  app->event_system = (NYA_EventSystem){
      .event_queue_mutex      = SDL_CreateMutex(),
      .event_queue            = nya_array_create(&app->global_allocator, NYA_Event),
      .event_queue_read_index = 0,
  };
}

void nya_system_events_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  SDL_DestroyMutex(app->event_system.event_queue_mutex);
  nya_array_destroy(&app->event_system.event_queue);
}

/*
 * ─────────────────────────────────────────────────────────
 * EVENT FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_event_drain_sdl_events(void) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) nya_event_register_sdl_event(event);
}

void nya_event_register_sdl_event(SDL_Event event) {
  NYA_Event nya_event = _nya_event_from_sdl_event(event);
  if (nya_event.type == NYA_EVENT_INVALID) return;

  nya_event_dispatch(nya_event);
}

void nya_event_dispatch(NYA_Event event) {
  NYA_App* app = nya_app_get_instance();

  event.timestamp = nya_clock_get_timestamp_ms();

  SDL_LockMutex(app->event_system.event_queue_mutex);
  nya_array_push_back(&app->event_system.event_queue, event);
  SDL_UnlockMutex(app->event_system.event_queue_mutex);

  _nya_event_notify_listeners(&event);
}

b8 nya_event_poll(OUT NYA_Event* out_event) {
  nya_assert(out_event);

  NYA_App* app = nya_app_get_instance();

  SDL_LockMutex(app->event_system.event_queue_mutex);
  NYA_EventArray* nya_array = &app->event_system.event_queue;

  if (app->event_system.event_queue_read_index >= nya_array->length) {
    nya_array_clear(nya_array);
    app->event_system.event_queue_read_index = 0;
    SDL_UnlockMutex(app->event_system.event_queue_mutex);
    return false;
  }

  *out_event = nya_array_get(nya_array, app->event_system.event_queue_read_index);
  app->event_system.event_queue_read_index++;
  SDL_UnlockMutex(app->event_system.event_queue_mutex);

  return true;
}

void nya_event_listen(NYA_EventHook hook) {
  nya_unused(hook);
  nya_todo();
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL NYA_Event _nya_event_from_sdl_event(SDL_Event sdl_event) {
  nya_unused(sdl_event);

  NYA_Event event = {0};

  switch (sdl_event.type) {
    case SDL_EVENT_KEY_DOWN: {
      event.type                        = NYA_EVENT_KEY_DOWN;
      event.as_key_event.window_id      = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.key.windowID);
      event.as_key_event.is_down        = true;
      event.as_key_event.is_repeat      = sdl_event.key.repeat != 0;
      event.as_key_event.key            = sdl_event.key.key;
      event.as_key_event.scancode       = (NYA_Scancode)sdl_event.key.scancode;
      event.as_key_event.modifier_flags = sdl_event.key.mod;
      event.as_key_event.raw            = sdl_event.key.raw;
    } break;

    case SDL_EVENT_KEY_UP: {
      event.type                        = NYA_EVENT_KEY_UP;
      event.as_key_event.window_id      = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.key.windowID);
      event.as_key_event.is_down        = false;
      event.as_key_event.is_repeat      = sdl_event.key.repeat != 0;
      event.as_key_event.key            = sdl_event.key.key;
      event.as_key_event.scancode       = (NYA_Scancode)sdl_event.key.scancode;
      event.as_key_event.modifier_flags = sdl_event.key.mod;
      event.as_key_event.raw            = sdl_event.key.raw;
    } break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
      event.type                            = NYA_EVENT_MOUSE_BUTTON_DOWN;
      event.as_mouse_button_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.button.windowID);
      event.as_mouse_button_event.is_down   = true;
      event.as_mouse_button_event.button    = sdl_event.button.button;
      event.as_mouse_button_event.x         = sdl_event.button.x;
      event.as_mouse_button_event.y         = sdl_event.button.y;
      event.as_mouse_button_event.clicks    = sdl_event.button.clicks;
    } break;

    case SDL_EVENT_MOUSE_BUTTON_UP: {
      event.type                            = NYA_EVENT_MOUSE_BUTTON_UP;
      event.as_mouse_button_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.button.windowID);
      event.as_mouse_button_event.is_down   = false;
      event.as_mouse_button_event.button    = sdl_event.button.button;
      event.as_mouse_button_event.x         = sdl_event.button.x;
      event.as_mouse_button_event.y         = sdl_event.button.y;
      event.as_mouse_button_event.clicks    = sdl_event.button.clicks;
    } break;

    case SDL_EVENT_MOUSE_MOTION: {
      event.type                           = NYA_EVENT_MOUSE_MOVED;
      event.as_mouse_moved_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.motion.windowID);
      event.as_mouse_moved_event.state     = sdl_event.motion.state;
      event.as_mouse_moved_event.x         = sdl_event.motion.x;
      event.as_mouse_moved_event.y         = sdl_event.motion.y;
      event.as_mouse_moved_event.delta_x   = sdl_event.motion.xrel;
      event.as_mouse_moved_event.delta_y   = sdl_event.motion.yrel;
    } break;

    case SDL_EVENT_MOUSE_WHEEL: {
      event.type                                  = NYA_EVENT_MOUSE_WHEEL_MOVED;
      event.as_mouse_wheel_event.window_id        = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.wheel.windowID);
      event.as_mouse_wheel_event.direction        = (NYA_MouseWheelDirection)sdl_event.wheel.direction;
      event.as_mouse_wheel_event.amount_x         = sdl_event.wheel.x;
      event.as_mouse_wheel_event.amount_y         = sdl_event.wheel.y;
      event.as_mouse_wheel_event.mouse_x          = sdl_event.wheel.mouse_x;
      event.as_mouse_wheel_event.mouse_y          = sdl_event.wheel.mouse_y;
      event.as_mouse_wheel_event.integer_amount_x = sdl_event.wheel.integer_x;
      event.as_mouse_wheel_event.integer_amount_y = sdl_event.wheel.integer_y;
    } break;

    case SDL_EVENT_QUIT: {
      event.type = NYA_EVENT_QUIT;
    } break;

    case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
      event.type                      = NYA_EVENT_WINDOW_CLOSE_REQUESTED;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_DESTROYED: {
      event.type                      = NYA_EVENT_WINDOW_DESTROYED;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: {
      event.type                      = NYA_EVENT_WINDOW_ENTER_FULLSCREEN;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_FOCUS_GAINED: {
      event.type                      = NYA_EVENT_WINDOW_FOCUS_GAINED;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_FOCUS_LOST: {
      event.type                      = NYA_EVENT_WINDOW_FOCUS_LOST;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_HIDDEN: {
      event.type                      = NYA_EVENT_WINDOW_HIDDEN;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: {
      event.type                      = NYA_EVENT_WINDOW_LEAVE_FULLSCREEN;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_MAXIMIZED: {
      event.type                      = NYA_EVENT_WINDOW_MAXIMIZED;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_MINIMIZED: {
      event.type                      = NYA_EVENT_WINDOW_MINIMIZED;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_MOUSE_ENTER: {
      event.type                      = NYA_EVENT_WINDOW_MOUSE_ENTER;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
      event.type                      = NYA_EVENT_WINDOW_MOUSE_LEAVE;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_MOVED: {
      event.type                            = NYA_EVENT_WINDOW_MOVED;
      event.as_window_moved_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
      event.as_window_moved_event.x         = sdl_event.window.data1;
      event.as_window_moved_event.y         = sdl_event.window.data2;
    } break;

    case SDL_EVENT_WINDOW_OCCLUDED: {
      event.type                      = NYA_EVENT_WINDOW_OCCLUDED;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_RESIZED: {
      event.type                              = NYA_EVENT_WINDOW_RESIZED;
      event.as_window_resized_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
      event.as_window_resized_event.width     = sdl_event.window.data1;
      event.as_window_resized_event.height    = sdl_event.window.data2;
    } break;

    case SDL_EVENT_WINDOW_RESTORED: {
      event.type                      = NYA_EVENT_WINDOW_RESTORED;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    case SDL_EVENT_WINDOW_SHOWN: {
      event.type                      = NYA_EVENT_WINDOW_SHOWN;
      event.as_window_event.window_id = _nya_event_sdl_window_id_to_nya_window_id(sdl_event.window.windowID);
    } break;

    default: {
      event.type = NYA_EVENT_INVALID;
    } break;
  }

  return event;
}

NYA_INTERNAL void* _nya_event_sdl_window_id_to_nya_window_id(SDL_WindowID sdl_window_id) {
  NYA_App* app = nya_app_get_instance();

  nya_array_foreach (&app->window_system.windows, nya_window) {
    if (SDL_GetWindowID(nya_window->sdl_window) == sdl_window_id) return nya_window->id;
  }

  return nullptr;
}

NYA_INTERNAL void _nya_event_notify_listeners(NYA_Event* event) {
  nya_unused(event);
}
