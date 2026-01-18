#include "nyangine/nyangine.h"

typedef struct {
  float x, y, z;
  float r, g, b, a;
} Vertex;
static Vertex vertices[] = {
    { 0.0F,  0.5F, 0.0F, 1.0F, 0.0F, 0.0F, 1.0F},
    {-0.5F, -0.5F, 0.0F, 1.0F, 1.0F, 0.0F, 1.0F},
    { 0.5F, -0.5F, 0.0F, 1.0F, 0.0F, 1.0F, 1.0F},
};

SDL_GPUBuffer* vertexBuffer;

void nya_renderer_init_for_window(NYA_App* app, NYA_Window* window) {
  nya_assert(app);
  nya_assert(window);

  b8 ok;

  ok = SDL_ClaimWindowForGPUDevice(app->gpu_device, window->sdl_window);
  nya_assert(ok, "SDL_ClaimWindowForGPUDevice() failed: %s", SDL_GetError());

  ok = SDL_SetGPUSwapchainParameters(
      app->gpu_device,
      window->sdl_window,
      SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
      app->config.vsync_enabled ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_MAILBOX
  );
  nya_assert(ok, "SDL_SetGPUSwapchainParameters() failed: %s", SDL_GetError());

  NYA_String vert_shader_string = nya_string_new(&nya_arena_global);
  NYA_String frag_shader_string = nya_string_new(&nya_arena_global);
  nya_file_read("./assets/shaders/compiled/SPIRV/sample.frag.spv", &frag_shader_string);
  nya_file_read("./assets/shaders/compiled/SPIRV/sample.vert.spv", &vert_shader_string);
  SDL_GPUShaderCreateInfo vertex_shader_info = {
      .code_size            = vert_shader_string.length,
      .code                 = vert_shader_string.items,
      .entrypoint           = "main",
      .format               = SDL_GPU_SHADERFORMAT_SPIRV,
      .stage                = SDL_GPU_SHADERSTAGE_VERTEX,
      .num_samplers         = 0,
      .num_storage_buffers  = 0,
      .num_storage_textures = 0,
      .num_uniform_buffers  = 0,
  };
  SDL_GPUShaderCreateInfo frag_shader_info = {
      .code_size            = frag_shader_string.length,
      .code                 = frag_shader_string.items,
      .entrypoint           = "main",
      .format               = SDL_GPU_SHADERFORMAT_SPIRV,
      .stage                = SDL_GPU_SHADERSTAGE_FRAGMENT,
      .num_samplers         = 0,
      .num_storage_buffers  = 0,
      .num_storage_textures = 0,
      .num_uniform_buffers  = 0,
  };
  SDL_GPUShader* vertex_shader = SDL_CreateGPUShader(app->gpu_device, &vertex_shader_info);
  nya_assert(vertex_shader, "SDL_CreateGPUShader() failed: %s", SDL_GetError());
  SDL_GPUShader* fragment_shader = SDL_CreateGPUShader(app->gpu_device, &frag_shader_info);
  nya_assert(fragment_shader, "SDL_CreateGPUShader() failed: %s", SDL_GetError());

  SDL_GPUVertexAttribute vertexAttributes[] = {
      {
       .location    = 0,
       .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
       .offset      = nya_offsetof(Vertex, x),
       .buffer_slot = 0,
       },
      {
       .location    = 1,
       .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
       .offset      = nya_offsetof(Vertex, r),
       .buffer_slot = 0,
       },
  };

  SDL_GPUVertexBufferDescription vertexBufferDescription = {
      .slot               = 0,
      .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
      .instance_step_rate = 0,
      .pitch              = sizeof(Vertex),
  };

  SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
      .target_info =
          {
                        .num_color_targets = 1,
                        .color_target_descriptions =
                  (SDL_GPUColorTargetDescription[]){
                      {.format = SDL_GetGPUSwapchainTextureFormat(app->gpu_device, window->sdl_window)}
                  }, },
      .primitive_type                                = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
      .vertex_shader                                 = vertex_shader,
      .fragment_shader                               = fragment_shader,
      .rasterizer_state.fill_mode                    = SDL_GPU_FILLMODE_FILL,
      .vertex_input_state.num_vertex_buffers         = 1,
      .vertex_input_state.vertex_buffer_descriptions = &vertexBufferDescription,
      .vertex_input_state.num_vertex_attributes      = 2,
      .vertex_input_state.vertex_attributes          = vertexAttributes,
  };
  window->simple_pipeline = SDL_CreateGPUGraphicsPipeline(app->gpu_device, &pipelineCreateInfo);

  nya_string_destroy(&vert_shader_string);
  nya_string_destroy(&frag_shader_string);
  SDL_ReleaseGPUShader(app->gpu_device, vertex_shader);
  SDL_ReleaseGPUShader(app->gpu_device, fragment_shader);

  SDL_GPUBufferCreateInfo bufferInfo = {
      .size  = sizeof(vertices),
      .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
  };
  vertexBuffer = SDL_CreateGPUBuffer(app->gpu_device, &bufferInfo);

  SDL_GPUTransferBufferCreateInfo transferInfo = {
      .size  = sizeof(vertices),
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
  };
  SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(app->gpu_device, &transferInfo);
  Vertex*                data           = SDL_MapGPUTransferBuffer(app->gpu_device, transferBuffer, false);
  nya_memcpy(data, vertices, sizeof(vertices));
  SDL_UnmapGPUTransferBuffer(app->gpu_device, transferBuffer);

  SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(app->gpu_device);
  SDL_GPUCopyPass*      copyPass      = SDL_BeginGPUCopyPass(commandBuffer);

  SDL_GPUTransferBufferLocation location = {
      .transfer_buffer = transferBuffer,
      .offset          = 0,
  };

  SDL_GPUBufferRegion region = {
      .buffer = vertexBuffer,
      .size   = sizeof(vertices),
      .offset = 0,
  };
  SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

  SDL_EndGPUCopyPass(copyPass);
  SDL_SubmitGPUCommandBuffer(commandBuffer);

  SDL_ReleaseGPUTransferBuffer(app->gpu_device, transferBuffer);
}

void nya_renderer_shutdown_for_window(NYA_App* app, NYA_Window* window) {
  nya_assert(app);
  nya_assert(window);

  SDL_WaitForGPUIdle(app->gpu_device);

  SDL_ReleaseGPUBuffer(app->gpu_device, vertexBuffer);
  SDL_ReleaseGPUGraphicsPipeline(app->gpu_device, window->simple_pipeline);

  SDL_ReleaseWindowFromGPUDevice(app->gpu_device, window->sdl_window);
}

void nya_renderer_draw(SDL_GPUDevice* gpu_device, NYA_Window* window) {
  nya_assert(gpu_device);
  nya_assert(window);

  SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(gpu_device);

  SDL_GPUTexture* swapchain_texture;
  SDL_AcquireGPUSwapchainTexture(
      command_buffer,
      window->sdl_window,
      &swapchain_texture,
      &window->width,
      &window->height
  );
  if (swapchain_texture == nullptr) return;

  SDL_GPUColorTargetInfo target_info = {
      .texture     = swapchain_texture,
      .clear_color = (SDL_FColor){0},
      .load_op     = SDL_GPU_LOADOP_CLEAR,
      .store_op    = SDL_GPU_STOREOP_STORE,
  };
  SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);
  nya_assert(render_pass != nullptr, "SDL_BeginGPURenderPass() failed");

  SDL_BindGPUGraphicsPipeline(render_pass, window->simple_pipeline);

  SDL_GPUBufferBinding bufferBindings[1] = {
      {.offset = 0, .buffer = vertexBuffer},
  };

  SDL_BindGPUVertexBuffers(render_pass, 0, bufferBindings, 1);

  SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

  SDL_EndGPURenderPass(render_pass);
  SDL_SubmitGPUCommandBuffer(command_buffer);
}
