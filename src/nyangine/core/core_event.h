#pragma once

#include "nyangine/base/base_ints.h"

typedef enum {
  NYA_EVENT_COUNT,
} NYA_EventType;

typedef struct {
  NYA_EventType type;
  b8            was_handled;
} NYA_Event;
