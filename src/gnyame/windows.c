#include "gnyame/gnyame.h"

void gny_window_main_create(void) {
  nya_window_create("gnyame", 1280, 720, NYA_WINDOW_RESIZABLE, GNY_WINDOW_MAIN);
  nya_window_set_minimum_size(GNY_WINDOW_MAIN, 1280, 720);

  nya_layer_push(GNY_WINDOW_MAIN, GNY_LAYER_GAME);
  nya_layer_push(GNY_WINDOW_MAIN, GNY_LAYER_UI);
}
