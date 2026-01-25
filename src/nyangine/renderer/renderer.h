#pragma once

#include "SDL3/SDL_gpu.h"

#include "nyangine/base/base.h"
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
  SDL_GPUCommandBuffer* command_buffer;
  SDL_GPUCopyPass*      copy_pass;
  SDL_GPURenderPass*    render_pass;
  SDL_GPUTexture*       swapchain_texture;

  SDL_GPUGraphicsPipeline* simple_pipeline;
};

/*
 * ─────────────────────────────────────────────────────────
 * RENDERING STRUCTS
 * ─────────────────────────────────────────────────────────
 */

struct NYA_Vertex {
  f32x3 position;
  f32x3 normals;
  f32x2 uv;
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
NYA_API NYA_EXTERN void nya_system_render_handle_event(NYA_Event* event);

/*
 * ─────────────────────────────────────────────────────────
 * RENDERING FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_render_set_vsync(b8 enabled);
NYA_API NYA_EXTERN void nya_render_begin(NYA_Window* window);
NYA_API NYA_EXTERN void nya_render_end(NYA_Window* window);

/*
 * - draw shapes: square, circle, line, triangle with a bit of control (color, thickness, etc)
 * - draw images and fonts
 * - apply fragment shaders to texture
 * - texture support: draw into a texture vs swapchain_texture and display it later
 * */
NYA_API NYA_EXTERN void nya_draw_clear(u32 color);
NYA_API NYA_EXTERN void nya_draw_rect(f32 x, f32 y, f32 width, f32 height, u32 color);
NYA_API NYA_EXTERN void nya_draw_vertices(NYA_Vertex* vertices, u32 vertex_count);
