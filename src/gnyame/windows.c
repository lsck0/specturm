#include "gnyame/gnyame.h"

void gny_window_main_create(void) {
  nya_window_create(GNY_WINDOW_MAIN_ID, GNY_WINDOW_MAIN_TITLE, GNY_WINDOW_MAIN_WIDTH, GNY_WINDOW_MAIN_HEIGHT, GNY_WINDOW_MAIN_FLAGS);
  nya_window_set_minimum_size(GNY_WINDOW_MAIN_ID, GNY_WINDOW_MAIN_WIDTH, GNY_WINDOW_MAIN_HEIGHT);

  nya_layer_push(GNY_WINDOW_MAIN_ID, GNY_LAYER_GAME);
  nya_layer_push(GNY_WINDOW_MAIN_ID, GNY_LAYER_UI);
}
