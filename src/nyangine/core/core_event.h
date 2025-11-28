#pragma once

#include "nyangine/base/base_ints.h"

typedef struct NYA_Event     NYA_Event;
typedef struct NYA_EventHook NYA_EventHook;

typedef enum {
  NYA_EVENT_COUNT,
} NYA_EventType;

struct NYA_Event {
  NYA_EventType type;
  b8            was_handled;
};

struct NYA_EventHook {
  void (*fn)(NYA_Event*);
  bool (*condition)(NYA_Event*);
};
