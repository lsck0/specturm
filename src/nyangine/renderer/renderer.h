#pragma once

#include "SDL3/SDL_gpu.h"

#include "nyangine/base/base.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_RenderSystem       NYA_RenderSystem;
typedef struct NYA_RenderSystemWindow NYA_RenderSystemWindow;

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_RenderSystem {
  SDL_GPUDevice* gpu_device;
};

struct NYA_RenderSystemWindow {
  SDL_GPUCommandBuffer* command_buffer;
  SDL_GPUCopyPass*      copy_pass;
  SDL_GPURenderPass*    render_pass;
  SDL_GPUTexture*       swapchain_texture;

  SDL_GPUGraphicsPipeline* simple_pipeline;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

typedef struct NYA_Window NYA_Window;

NYA_API NYA_EXTERN void nya_system_render_init(void);
NYA_API NYA_EXTERN void nya_system_render_deinit(void);
NYA_API NYA_EXTERN void nya_system_render_for_window_init(NYA_Window* window);
NYA_API NYA_EXTERN void nya_system_render_for_window_deinit(NYA_Window* window);

/*
 * ─────────────────────────────────────────────────────────
 * RENDERING FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_render_begin(NYA_Window* window);
NYA_API NYA_EXTERN void nya_render_end(NYA_Window* window);
