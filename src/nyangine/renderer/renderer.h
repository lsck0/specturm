#pragma once

#include "SDL3/SDL_gpu.h"

#include "nyangine/core/core_app.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_renderer_init_for_window(NYA_App* app, NYA_Window* window);
NYA_API NYA_EXTERN void nya_renderer_shutdown_for_window(NYA_App* app, NYA_Window* window);

NYA_API NYA_EXTERN void nya_renderer_draw(SDL_GPUDevice* gpu_device, NYA_Window* window);
