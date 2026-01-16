#pragma once

#include "SDL3/SDL_events.h"

#include "nyangine/base/base_array.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_Event     NYA_Event;
typedef struct NYA_EventHook NYA_EventHook;
nya_derive_array(NYA_Event);

typedef enum {
  NYA_EVENT_COUNT,
} NYA_EventType;

struct NYA_Event {
  NYA_EventType type;
  b8            was_handled;

  union {};
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

NYA_API NYA_EXTERN NYA_Event nya_event_from_sdl_event(SDL_Event* sdl_event);
NYA_API NYA_EXTERN void      nya_event_emit(NYA_Event event);
NYA_API NYA_EXTERN b8        nya_event_poll(OUT NYA_Event* out_event);
