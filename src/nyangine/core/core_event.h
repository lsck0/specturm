#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_mutex.h"

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_hmap.h"
#include "nyangine/base/base_keys.h"
#include "nyangine/base/base_mouse.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef enum NYA_EventType            NYA_EventType;
typedef enum NYA_EventHookType        NYA_EventHookType;
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
nya_derive_array(NYA_EventHook);
nya_derive_hmap(NYA_EventType, NYA_EventHookArray);

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

  NYA_EventType_NYA_EventHookArray_HMap deferred_event_hooks;
  NYA_EventType_NYA_EventHookArray_HMap immediate_event_hooks;
};

/*
 * ─────────────────────────────────────────────────────────
 * EVENT TYPE ENUM
 * ─────────────────────────────────────────────────────────
 */

enum NYA_EventType {
  NYA_EVENT_INVALID,

  NYA_EVENT_LIFECYCLE_EVENTS_BEGIN,
  NYA_EVENT_FRAME_STARTED,
  NYA_EVENT_FRAME_ENDED,
  NYA_EVENT_HANDLING_STARTED,
  NYA_EVENT_HANDLING_ENDED,
  NYA_EVENT_UPDATING_STARTED,
  NYA_EVENT_UPDATING_ENDED,
  NYA_EVENT_RENDERING_STARTED,
  NYA_EVENT_RENDERING_ENDED,
  NYA_EVENT_LIFECYCLE_EVENTS_END,

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
};

__attr_unused static NYA_ConstCString NYA_EVENT_NAME_MAP[] = {
  [NYA_EVENT_INVALID] = "INVALID",

  [NYA_EVENT_FRAME_STARTED]     = "FRAME_STARTED",
  [NYA_EVENT_FRAME_ENDED]       = "FRAME_ENDED",
  [NYA_EVENT_HANDLING_STARTED]  = "HANDLING_STARTED",
  [NYA_EVENT_HANDLING_ENDED]    = "HANDLING_ENDED",
  [NYA_EVENT_UPDATING_STARTED]  = "UPDATING_STARTED",
  [NYA_EVENT_UPDATING_ENDED]    = "UPDATING_ENDED",
  [NYA_EVENT_RENDERING_STARTED] = "RENDERING_STARTED",
  [NYA_EVENT_RENDERING_ENDED]   = "RENDERING_ENDED",

  [NYA_EVENT_KEY_DOWN] = "KEY_DOWN",
  [NYA_EVENT_KEY_UP]   = "KEY_UP",

  [NYA_EVENT_MOUSE_BUTTON_DOWN] = "MOUSE_BUTTON_DOWN",
  [NYA_EVENT_MOUSE_BUTTON_UP]   = "MOUSE_BUTTON_UP",
  [NYA_EVENT_MOUSE_MOVED]       = "MOUSE_MOVED",
  [NYA_EVENT_MOUSE_WHEEL_MOVED] = "MOUSE_WHEEL_MOVED",

  [NYA_EVENT_QUIT] = "QUIT",

  [NYA_EVENT_WINDOW_CLOSE_REQUESTED]  = "WINDOW_CLOSE_REQUESTED",
  [NYA_EVENT_WINDOW_DESTROYED]        = "WINDOW_DESTROYED",
  [NYA_EVENT_WINDOW_ENTER_FULLSCREEN] = "WINDOW_ENTER_FULLSCREEN",
  [NYA_EVENT_WINDOW_FOCUS_GAINED]     = "WINDOW_FOCUS_GAINED",
  [NYA_EVENT_WINDOW_FOCUS_LOST]       = "WINDOW_FOCUS_LOST",
  [NYA_EVENT_WINDOW_HIDDEN]           = "WINDOW_HIDDEN",
  [NYA_EVENT_WINDOW_LEAVE_FULLSCREEN] = "WINDOW_LEAVE_FULLSCREEN",
  [NYA_EVENT_WINDOW_MAXIMIZED]        = "WINDOW_MAXIMIZED",
  [NYA_EVENT_WINDOW_MINIMIZED]        = "WINDOW_MINIMIZED",
  [NYA_EVENT_WINDOW_MOUSE_ENTER]      = "WINDOW_MOUSE_ENTER",
  [NYA_EVENT_WINDOW_MOUSE_LEAVE]      = "WINDOW_MOUSE_LEAVE",
  [NYA_EVENT_WINDOW_MOVED]            = "WINDOW_MOVED",
  [NYA_EVENT_WINDOW_OCCLUDED]         = "WINDOW_OCCLUDED",
  [NYA_EVENT_WINDOW_RESIZED]          = "WINDOW_RESIZED",
  [NYA_EVENT_WINDOW_RESTORED]         = "WINDOW_RESTORED",
  [NYA_EVENT_WINDOW_SHOWN]            = "WINDOW_SHOWN",
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
    NYA_KeyEvent           as_key_event;
    NYA_MouseButtonEvent   as_mouse_button_event;
    NYA_MouseMovedEvent    as_mouse_moved_event;
    NYA_MouseWheelEvent    as_mouse_wheel_event;
    NYA_WindowEvent        as_window_event;
    NYA_WindowMovedEvent   as_window_moved_event;
    NYA_WindowResizedEvent as_window_resized_event;
  };
};

enum NYA_EventHookType {
  NYA_EVENT_HOOK_TYPE_DEFERRED,
  NYA_EVENT_HOOK_TYPE_IMMEDIATE,
  NYA_EVENT_HOOK_TYPE_COUNT,
};

struct NYA_EventHook {
  NYA_EventType     event_type;
  NYA_EventHookType hook_type;
  void (*fn)(NYA_Event*);
  b8 (*condition)(NYA_Event*);
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
NYA_API NYA_EXTERN void nya_system_event_drain_sdl_events(void);
NYA_API NYA_EXTERN b8   nya_system_event_poll(OUT NYA_Event* out_event);

/*
 * ─────────────────────────────────────────────────────────
 * EVENT FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_event_dispatch(NYA_Event event);
NYA_API NYA_EXTERN void nya_event_hook_register(NYA_EventHook hook);
NYA_API NYA_EXTERN void nya_event_hook_unregister(NYA_EventHook hook);
