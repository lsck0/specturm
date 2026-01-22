#pragma once

#include "nyangine/base/base_types.h"

typedef u8 NYA_MouseButton;

#define NYA_MOUSE_BUTTON_LEFT   1
#define NYA_MOUSE_BUTTON_MIDDLE 2
#define NYA_MOUSE_BUTTON_RIGHT  3
#define NYA_MOUSE_BUTTON_X1     4
#define NYA_MOUSE_BUTTON_X2     5
#define NYA_MOUSE_BUTTON_COUNT  6

typedef enum {
  NYA_MOUSE_BUTTON_FLAG_NONE   = 0,
  NYA_MOUSE_BUTTON_FLAG_LEFT   = (1 << 0),
  NYA_MOUSE_BUTTON_FLAG_MIDDLE = (1 << 1),
  NYA_MOUSE_BUTTON_FLAG_RIGHT  = (1 << 2),
  NYA_MOUSE_BUTTON_FLAG_X1     = (1 << 3),
  NYA_MOUSE_BUTTON_FLAG_X2     = (1 << 4),
} NYA_MouseButtonFlags;
