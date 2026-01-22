#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_mutex.h"

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_keys.h"
#include "nyangine/base/base_mouse.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_AssetEvent         NYA_AssetEvent;
typedef struct NYA_Event              NYA_Event;
typedef struct NYA_EventHook          NYA_EventHook;
typedef struct NYA_EventSystem        NYA_EventSystem;
typedef struct NYA_KeyEvent           NYA_KeyEvent;
typedef struct NYA_MouseButtonEvent   NYA_MouseButtonEvent;
typedef struct NYA_MouseMovedEvent    NYA_MouseMovedEvent;
typedef struct NYA_MouseWheelEvent    NYA_MouseWheelEvent;
typedef struct NYA_WindowEvent        NYA_WindowEvent;
typedef struct NYA_WindowMovedEvent   NYA_WindowMovedEvent;
typedef struct NYA_WindowResizedEvent NYA_WindowResizedEvent;
nya_derive_array(NYA_Event);

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_EventSystem {
  NYA_Arena allocator;

  SDL_Mutex*     event_queue_mutex;
  NYA_EventArray event_queue;
  u64            event_queue_read_index;
};

/*
 * ─────────────────────────────────────────────────────────
 * EVENT TYPE ENUM
 * ─────────────────────────────────────────────────────────
 */

typedef enum {
  NYA_EVENT_INVALID = 0,

  NYA_EVENT_ASSET_LOADED,
  NYA_EVENT_ASSET_UPDATED,

  NYA_EVENT_NEW_TICK,

  NYA_EVENT_KEY_DOWN,
  NYA_EVENT_KEY_UP,

  NYA_EVENT_MOUSE_BUTTON_DOWN,
  NYA_EVENT_MOUSE_BUTTON_UP,
  NYA_EVENT_MOUSE_MOVED,
  NYA_EVENT_MOUSE_WHEEL_MOVED,

  NYA_EVENT_QUIT,

  NYA_EVENT_WINDOW_CLOSE_REQUESTED,
  NYA_EVENT_WINDOW_DESTROYED,
  NYA_EVENT_WINDOW_ENTER_FULLSCREEN,
  NYA_EVENT_WINDOW_FOCUS_GAINED,
  NYA_EVENT_WINDOW_FOCUS_LOST,
  NYA_EVENT_WINDOW_HIDDEN,
  NYA_EVENT_WINDOW_LEAVE_FULLSCREEN,
  NYA_EVENT_WINDOW_MAXIMIZED,
  NYA_EVENT_WINDOW_MINIMIZED,
  NYA_EVENT_WINDOW_MOUSE_ENTER,
  NYA_EVENT_WINDOW_MOUSE_LEAVE,
  NYA_EVENT_WINDOW_MOVED,
  NYA_EVENT_WINDOW_OCCLUDED,
  NYA_EVENT_WINDOW_RESIZED,
  NYA_EVENT_WINDOW_RESTORED,
  NYA_EVENT_WINDOW_SHOWN,

  NYA_EVENT_COUNT,
} NYA_EventType;

/*
 * ─────────────────────────────────────────────────────────
 * ASSET EVENT STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_AssetEvent {
  NYA_ConstCString path;
};

/*
 * ─────────────────────────────────────────────────────────
 * KEYBOARD EVENT STRUCTS
 * ─────────────────────────────────────────────────────────
 */

struct NYA_KeyEvent {
  void*          window_id;
  b8             is_down;
  b8             is_repeat;
  NYA_Keycode    key;
  NYA_Scancode   scancode;
  NYA_KeyModFlag modifier_flags;
  u16            raw;
};

/*
 * ─────────────────────────────────────────────────────────
 * MOUSE EVENT STRUCTS
 * ─────────────────────────────────────────────────────────
 */

typedef enum {
  NYA_MOUSE_WHEEL_DIRECTION_NORMAL  = SDL_MOUSEWHEEL_NORMAL,
  NYA_MOUSE_WHEEL_DIRECTION_FLIPPED = SDL_MOUSEWHEEL_FLIPPED,
} NYA_MouseWheelDirection;

struct NYA_MouseButtonEvent {
  void*           window_id;
  b8              is_down;
  NYA_MouseButton button;
  u8              clicks;
  f32             x;
  f32             y;
};

struct NYA_MouseMovedEvent {
  void*                window_id;
  NYA_MouseButtonFlags state;
  f32                  x;
  f32                  y;
  f32                  delta_x;
  f32                  delta_y;
};

struct NYA_MouseWheelEvent {
  void*                   window_id;
  NYA_MouseWheelDirection direction;
  f32                     amount_x;
  f32                     amount_y;
  f32                     mouse_x;
  f32                     mouse_y;
  s32                     integer_amount_x;
  s32                     integer_amount_y;
};

/*
 * ─────────────────────────────────────────────────────────
 * WINDOW EVENT STRUCTS
 * ─────────────────────────────────────────────────────────
 */

struct NYA_WindowEvent {
  void* window_id;
};

struct NYA_WindowMovedEvent {
  void* window_id;
  u32   x;
  u32   y;
};

struct NYA_WindowResizedEvent {
  void* window_id;
  u32   width;
  u32   height;
};

/*
 * ─────────────────────────────────────────────────────────
 * EVENT STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_Event {
  NYA_EventType type;
  b8            was_handled;
  u64           timestamp;

  union {
    NYA_AssetEvent         as_asset_event;
    NYA_KeyEvent           as_key_event;
    NYA_MouseButtonEvent   as_mouse_button_event;
    NYA_MouseMovedEvent    as_mouse_moved_event;
    NYA_MouseWheelEvent    as_mouse_wheel_event;
    NYA_WindowEvent        as_window_event;
    NYA_WindowMovedEvent   as_window_moved_event;
    NYA_WindowResizedEvent as_window_resized_event;
  };
};

struct NYA_EventHook {
  void (*fn)(NYA_Event*);
  bool (*condition)(NYA_Event*);
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

NYA_API NYA_EXTERN void nya_system_events_init(void);
NYA_API NYA_EXTERN void nya_system_events_deinit(void);

/*
 * ─────────────────────────────────────────────────────────
 * EVENT FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_event_drain_sdl_events(void);
NYA_API NYA_EXTERN void nya_event_register_sdl_event(SDL_Event event);
NYA_API NYA_EXTERN void nya_event_dispatch(NYA_Event event);
NYA_API NYA_EXTERN b8   nya_event_poll(OUT NYA_Event* out_event);
NYA_API NYA_EXTERN void nya_event_listen(NYA_EventHook hook);
