#pragma once

#include "nyangine/nyangine.h"

void*           GNY_WINDOW_MAIN_ID     = "gny_window_main";
NYA_CString     GNY_WINDOW_MAIN_TITLE  = "gnyame";
u32             GNY_WINDOW_MAIN_WIDTH  = 1280;
u32             GNY_WINDOW_MAIN_HEIGHT = 720;
NYA_WindowFlags GNY_WINDOW_MAIN_FLAGS  = NYA_WINDOW_RESIZABLE;
void            gny_window_main_create(void);
