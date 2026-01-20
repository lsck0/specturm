#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_system_render_init(void) {
  NYA_App* app = nya_app_get_instance();

  SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(
      SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB | SDL_GPU_SHADERFORMAT_SPIRV,
      NYA_IS_DEBUG,
      nullptr
  );
  nya_assert(gpu_device != nullptr, "SDL_CreateGPUDevice() failed: %s", SDL_GetError());

  app->render_system = (NYA_RenderSystem){
      .gpu_device = gpu_device,
  };
}

void nya_system_render_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  SDL_WaitForGPUIdle(app->render_system.gpu_device);
  SDL_DestroyGPUDevice(app->render_system.gpu_device);
}

void nya_system_render_for_window_init(NYA_Window* window) {
  nya_assert(window);

  NYA_App* app = nya_app_get_instance();

  b8 ok;

  ok = SDL_ClaimWindowForGPUDevice(app->render_system.gpu_device, window->sdl_window);
  nya_assert(ok, "SDL_ClaimWindowForGPUDevice() failed: %s", SDL_GetError());

  ok = SDL_SetGPUSwapchainParameters(
      app->render_system.gpu_device,
      window->sdl_window,
      SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
      app->config.vsync_enabled ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_MAILBOX
  );
  nya_assert(ok, "SDL_SetGPUSwapchainParameters() failed: %s", SDL_GetError());

  window->render_system = (NYA_RenderSystemWindow){0};
}

void nya_system_render_for_window_deinit(NYA_Window* window) {
  nya_assert(window);

  NYA_App* app = nya_app_get_instance();

  SDL_WaitForGPUIdle(app->render_system.gpu_device);
  SDL_ReleaseWindowFromGPUDevice(app->render_system.gpu_device, window->sdl_window);
}

/*
 * ─────────────────────────────────────────────────────────
 * RENDERING FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_render_begin(NYA_Window* window) {
  nya_assert(window);

  NYA_App* app = nya_app_get_instance();

  SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(app->render_system.gpu_device);
  window->render_system.command_buffer = command_buffer;

  SDL_GPUTexture* swapchain_texture;
  SDL_AcquireGPUSwapchainTexture(
      command_buffer,
      window->sdl_window,
      &swapchain_texture,
      &window->screen_width,
      &window->screed_height
  );
  window->render_system.swapchain_texture = swapchain_texture;
  if (swapchain_texture == nullptr) return;

  SDL_GPUColorTargetInfo target_info = {
      .texture     = swapchain_texture,
      .clear_color = (SDL_FColor){0},
      .load_op     = SDL_GPU_LOADOP_CLEAR,
      .store_op    = SDL_GPU_STOREOP_STORE,
  };
  SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);
  nya_assert(render_pass != nullptr, "SDL_BeginGPURenderPass() failed");
  window->render_system.render_pass = render_pass;
}

void nya_render_end(NYA_Window* window) {
  nya_assert(window);

  SDL_EndGPURenderPass(window->render_system.render_pass);
  SDL_SubmitGPUCommandBuffer(window->render_system.command_buffer);
}
