#pragma once

#include "gnyame/gnyame.h"

void gny_layers_init(void);

NYA_Layer GNY_LAYER_GAME;
void      gny_layer_game_on_create(void);
void      gny_layer_game_on_destroy(void);
void      gny_layer_game_on_event(NYA_Window* window, NYA_Event* event);
void      gny_layer_game_on_update(NYA_Window* window, f32 delta_time_s);
void      gny_layer_game_on_render(NYA_Window* window);

NYA_Layer GNY_LAYER_UI;
void      gny_layer_ui_on_create(void);
void      gny_layer_ui_on_destroy(void);
void      gny_layer_ui_on_event(NYA_Window* window, NYA_Event* event);
void      gny_layer_ui_on_update(NYA_Window* window, f32 delta_time_s);
void      gny_layer_ui_on_render(NYA_Window* window);
