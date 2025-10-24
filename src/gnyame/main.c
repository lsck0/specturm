#include "SDL3/SDL.h"

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  SDL_Window*    window     = SDL_CreateWindow("gnyame", 1280, 720, SDL_WINDOW_RESIZABLE);
  SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(
      SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB | SDL_GPU_SHADERFORMAT_SPIRV,
      true,
      nullptr
  );

  SDL_ClaimWindowForGPUDevice(gpu_device, window);

  SDL_SetGPUSwapchainParameters(gpu_device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);

  bool should_quit = false;
  while (!should_quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) should_quit = true;
      if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)) {
        should_quit = true;
      }
    }

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(gpu_device);

    SDL_GPUTexture* swapchain_texture;
    SDL_AcquireGPUSwapchainTexture(command_buffer, window, &swapchain_texture, nullptr, nullptr);

    if (swapchain_texture != nullptr) {
      SDL_GPUColorTargetInfo target_info = {
          .texture     = swapchain_texture,
          .clear_color = (SDL_FColor){0},
          .load_op     = SDL_GPU_LOADOP_CLEAR,
          .store_op    = SDL_GPU_STOREOP_STORE,
      };
      SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

      SDL_EndGPURenderPass(render_pass);
    }

    SDL_SubmitGPUCommandBuffer(command_buffer);
  }

  SDL_WaitForGPUIdle(gpu_device);
  SDL_ReleaseWindowFromGPUDevice(gpu_device, window);
  SDL_DestroyGPUDevice(gpu_device);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
