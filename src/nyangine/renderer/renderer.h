#pragma once

#include "SDL3/SDL_gpu.h"

#include "nyangine/base/base.h"
#include "nyangine/base/base_color.h"
#include "nyangine/base/base_types.h"
#include "nyangine/core/core_event.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_RenderSystem       NYA_RenderSystem;
typedef struct NYA_RenderSystemWindow NYA_RenderSystemWindow;
typedef struct NYA_Vertex             NYA_Vertex;

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_RenderSystem {
  SDL_GPUDevice* gpu_device;
};

struct NYA_RenderSystemWindow {
  SDL_GPURenderPass*    render_pass;
  SDL_GPUCommandBuffer* render_commands;
  SDL_GPUTexture*       swapchain_texture;
};

/*
 * ─────────────────────────────────────────────────────────
 * RENDERING STRUCTS
 * ─────────────────────────────────────────────────────────
 */

struct NYA_Vertex {
  f32x3     position;
  NYA_Color color;
  f32x3     normals;
  f32x2     uv;
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

NYA_API NYA_EXTERN void nya_system_renderer_init(void);
NYA_API NYA_EXTERN void nya_system_renderer_deinit(void);
NYA_API NYA_EXTERN void nya_system_renderer_for_window_init(NYA_Window* window);
NYA_API NYA_EXTERN void nya_system_renderer_for_window_deinit(NYA_Window* window);
NYA_API NYA_EXTERN void nya_system_renderer_set_vsync(b8 enabled);

/*
 * ─────────────────────────────────────────────────────────
 * RENDERING FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_render_begin(NYA_Window* window);
NYA_API NYA_EXTERN void nya_render_end(NYA_Window* window);
