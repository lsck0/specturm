#pragma once

#include "nyangine/core/core_event.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_Entity NYA_Entity;

typedef enum NYA_EntityFlag {
  NYA_ENTITY_FLAG_NONE = 0,
} NYA_EntityFlag;

struct NYA_Entity {
  NYA_EntityFlag flags;

  void (*on_create)(void);
  void (*on_destroy)(void);
  void (*on_update)(f32 delta_time);
  void (*on_event)(NYA_Event* event);
  void (*on_render)(void);
  NYA_EventHook event_hooks[NYA_EVENT_COUNT];
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */
