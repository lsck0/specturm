#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLRARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

// Forward declarations
NYA_Asset*        nya_asset_get_struct(NYA_AssetHandle handle);
NYA_INTERNAL void _nya_asset_create_gpu_resource(NYA_Asset* asset, NYA_AssetLoadParams params);
#ifdef NYA_ASSET_BACKEND_BLOB
NYA_INTERNAL void _nya_asset_load_from_blob(NYA_Asset* asset);
#endif
#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL NYA_Asset _nya_asset_load_from_filesystem(NYA_AssetHandle handle, NYA_AssetType type);
#endif
#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL NYA_Asset _nya_asset_load_from_filesystem(NYA_AssetHandle path, NYA_AssetType type);
#endif
#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL NYA_Asset _nya_asset_load_from_filesystem(NYA_CString path, NYA_AssetType type);
#endif

#define SDL_GPU_SHADERSTAGE_COMPUTE 2
#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL NYA_Asset _nya_asset_load_from_filesystem(NYA_CString path, NYA_AssetType type);
#endif

#if defined(NYA_ASSET_BACKEND_FS) && defined(NYA_ASSET_BACKEND_BLOB)
#error "Only one asset backend can be defined at a time."
#elif !defined(NYA_ASSET_BACKEND_FS) && !defined(NYA_ASSET_BACKEND_BLOB)
#error "An asset backend must be defined."
#endif

#ifdef NYA_ASSET_BACKEND_BLOB
#include "../../../assets/assets.c"

NYA_INTERNAL void _nya_asset_load_from_blob(NYA_Asset* asset);
#endif // NYA_ASSET_BACKEND_BLOB

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL NYA_Asset _nya_asset_load_from_filesystem(NYA_AssetHandle handle, NYA_AssetType type) {
  nya_assert(handle);

  if (!nya_filesystem_exists(handle)) {
    nya_panic("Asset not found in filesystem: %s", handle);
    nya_unreachable();
  }

  if (type == NYA_ASSET_TYPE_BUFFER_VERTEX || type == NYA_ASSET_TYPE_BUFFER_INDEX || type == NYA_ASSET_TYPE_BUFFER_UNIFORM ||
      type == NYA_ASSET_TYPE_GRAPHICS_PIPELINE) {
    u64 modification_time = 0;
    b8  ok                = nya_filesystem_last_modified(handle, &modification_time);
    nya_assert(ok);
    return (NYA_Asset){
      .type             = type,
      .status           = NYA_ASSET_STATUS_LOADED,
      .handle           = handle,
      .data             = nullptr,
      .size             = 0,
      .source_timestamp = modification_time,
      .reference_count  = 0,
    };
  } else {
    NYA_App*   app   = nya_app_get_instance();
    NYA_Arena* arena = &app->asset_system.allocator;

    NYA_String content = nya_string_create(arena);
    b8         ok      = nya_file_read(handle, &content);
    nya_assert(ok);

    u64 modification_time = 0;
    ok                    = nya_filesystem_last_modified(handle, &modification_time);
    nya_assert(ok);

    return (NYA_Asset){
      .type             = type,
      .status           = NYA_ASSET_STATUS_LOADED,
      .handle           = handle,
      .data             = content.items,
      .size             = content.length,
      .source_timestamp = modification_time,
      .reference_count  = 0,
    };
  }
}
#endif // NYA_ASSET_BACKEND_FS

#if OS_WINDOWS
#define NYA_SHADER_FORMAT         "DXIL"
#define NYA_SHADER_PATH_SUFFIX    ".dxil"
#define NYA_SHADER_COMPILE_FORMAT SDL_GPU_SHADERFORMAT_DXIL
#elif OS_MAC
#define NYA_SHADER_FORMAT         "MSL"
#define NYA_SHADER_PATH_SUFFIX    ".msl"
#define NYA_SHADER_COMPILE_FORMAT SDL_GPU_SHADERFORMAT_MSL
#elif OS_LINUX || OS_WASM
#define NYA_SHADER_FORMAT         "SPIRV"
#define NYA_SHADER_PATH_SUFFIX    ".spv"
#define NYA_SHADER_COMPILE_FORMAT SDL_GPU_SHADERFORMAT_SPIRV
#else
#error "Unsupported OS for SDL3 GPU API."
#endif

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

void nya_system_asset_init(void) {
  NYA_App* app = nya_app_get_instance();

  app->asset_system = (NYA_AssetSystem){
    .allocator = nya_arena_create(.name = "asset_system_allocator"),
  };

  app->asset_system.assets         = nya_hmap_create(&app->asset_system.allocator, NYA_AssetHandle, NYA_Asset);
  app->asset_system.changed_assets = nya_array_create(&app->asset_system.allocator, NYA_CString);
  app->asset_system.batch_types    = nya_array_create(&app->asset_system.allocator, u32);
  app->asset_system.batch_requests = nya_array_create(&app->asset_system.allocator, NYA_AssetLoadParams);

  nya_info("Asset system initialized.");
}

void nya_system_asset_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  nya_array_destroy(&app->asset_system.batch_types);
  nya_array_destroy(&app->asset_system.batch_requests);
  nya_array_destroy(&app->asset_system.changed_assets);
  nya_hmap_destroy(&app->asset_system.assets);

  nya_arena_destroy(&app->asset_system.allocator);

  nya_info("Asset system deinitialized.");
}

void nya_system_asset_handle_event(NYA_Event* event) {
  nya_assert(event);

  NYA_App* app = nya_app_get_instance();

  if (event->type == NYA_EVENT_UPDATING_STARTED) {
    nya_array_foreach (&app->asset_system.changed_assets, asset_handle) {
      NYA_Asset* asset = nya_asset_get_struct(*asset_handle);
      if (!asset) continue;
      nya_asset_release(*asset_handle);
      // Force reload
      asset->status = NYA_ASSET_STATUS_UNLOADED;
      asset->data   = nullptr;
      asset->size   = 0;
      memset(&asset->gpu_resource, 0, sizeof(asset->gpu_resource));
      nya_asset_load_and_acquire(NYA_ASSET_TYPE_TEXT, *asset_handle, (NYA_AssetLoadParams){ .handle = *asset_handle });
    }
    nya_array_clear(&app->asset_system.changed_assets);
  }
}

/*
 * ─────────────────────────────────────────────────────────
 * ASSET FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_asset_generate_manifest(NYA_ConstCString asset_directory, NYA_ConstCString output_file) {
  nya_assert(asset_directory);
  nya_assert(output_file);

  NYA_Arena* arena  = &nya_arena_global;
  NYA_String result = nya_string_create(arena);

  NYA_Command find_assets_command = {
      .arena     = arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "find",
      .arguments = {
          asset_directory,
          "-type", "f",
          "-not", "-name", "*.c",
          "-not", "-name", "*.h",
          "-not", "-name", ".keep",
      },
  };
  nya_command_run(&find_assets_command);
  NYA_StringArray files = nya_string_split_lines(arena, &find_assets_command.stdout_content);
  nya_string_extend(&result, "/* THIS FILE IS GENERATED. DO NYAT TOUCH. */\n\n");
  nya_string_extend(&result, "#pragma once\n\n");

  nya_array_foreach (&files, file) {
    NYA_String var_name = nya_string_clone(arena, file);
    nya_string_strip_prefix(&var_name, "./");
    nya_string_replace(&var_name, "/", "_");
    nya_string_replace(&var_name, ".", "_");
    nya_string_replace(&var_name, "-", "_");
    nya_string_replace(&var_name, " ", "_");
    nya_string_to_upper(&var_name);

    NYA_String declaration = nya_string_sprintf(
        arena,
        "char* NYA_" NYA_FMT_STRING " = \"" NYA_FMT_STRING "\";\n",
        NYA_FMT_STRING_ARG(var_name),
        NYA_FMT_STRING_ARG(*file)
    );
    nya_string_extend(&result, &declaration);
  }

  b8 ok = nya_file_write(output_file, &result);
  nya_assert(ok);

  NYA_Command format_command = {
    .program   = "clang-format",
    .arguments = { "-i", output_file, },
  };
  nya_command_run(&format_command);
}

void nya_asset_generate_embedding(NYA_ConstCString asset_directory, NYA_ConstCString output_file) {
  nya_assert(asset_directory);
  nya_assert(output_file);

  NYA_Arena* arena               = &nya_arena_global;
  NYA_String result              = nya_string_create(arena);
  NYA_String header_count_string = nya_string_create(arena);
  NYA_String header_string       = nya_string_create(arena);
  NYA_String blob_string         = nya_string_create(arena);

  NYA_Command find_assets_command = {
      .arena     = arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "find",
      .arguments = {
          asset_directory,
          "-type", "f",
          "-not", "-name", "*.c",
          "-not", "-name", "*.h",
          "-not", "-name", ".keep",
      },
  };
  nya_command_run(&find_assets_command);
  NYA_StringArray files = nya_string_split_lines(arena, &find_assets_command.stdout_content);
  nya_string_extend(&result, "/* THIS FILE IS GENERATED. DO NYAT TOUCH. */\n\n");
  nya_string_extend(&result, "#include \"nyangine/nyangine.h\"\n\n");
  header_count_string = nya_string_sprintf(arena, "static const u64 NYA_ASSET_BLOB_HEADER_COUNT = " FMTu64 ";\n", files.length);
  nya_string_extend(&header_string, "static const NYA_AssetBlobHeader NYA_ASSET_BLOB_HEADER[] = {\n");
  nya_string_extend(&blob_string, "static const u8 NYA_ASSET_BLOB[] = {\n");

  u64 cursor = 0;
  nya_array_foreach (&files, file) {
    NYA_String content = nya_string_create(arena);
    b8         ok      = nya_file_read(file, &content);
    nya_assert(ok);

    NYA_String header_element_string =
        nya_string_sprintf(arena, "  { \"%.*s\", " FMTu64 ", " FMTu64 " },\n", NYA_FMT_STRING_ARG(*file), cursor, content.length);
    nya_string_extend(&header_string, &header_element_string);

    nya_array_foreach (&content, c) {
      NYA_String new = nya_string_sprintf(arena, "0x%02X,\n", *c);
      nya_string_extend(&blob_string, &new);
    }

    cursor += content.length;
  }
  nya_string_extend(&blob_string, "};\n\n");
  nya_string_extend(&header_string, "};\n\n");

  nya_string_extend(&result, &header_count_string);
  nya_string_extend(&result, &header_string);
  nya_string_extend(&result, &blob_string);

  b8 ok = nya_file_write(output_file, &result);
  nya_assert(ok);

  NYA_Command format_command = {
    .program   = "clang-format",
    .arguments = { "-i", output_file, },
  };
  nya_command_run(&format_command);
}

NYA_Asset* nya_asset_get_struct(NYA_AssetHandle handle) {
  NYA_App* app = nya_app_get_instance();
  return nya_hmap_get(&app->asset_system.assets, handle);
}

void nya_asset_load(NYA_AssetType type, NYA_AssetHandle handle, NYA_AssetLoadParams params) {
  nya_assert(handle);

  NYA_App*   app   = nya_app_get_instance();
  NYA_Asset* asset = nya_asset_get_struct(handle);

  if (!asset) {
    NYA_Asset new_asset = {
      .type             = type,
      .status           = NYA_ASSET_STATUS_UNLOADED,
      .handle           = handle,
      .data             = nullptr,
      .size             = 0,
      .source_timestamp = 0,
      .reference_count  = 0,
    };
    memset(&new_asset.gpu_resource, 0, sizeof(new_asset.gpu_resource));
    nya_hmap_set(&app->asset_system.assets, handle, new_asset);
    asset = nya_hmap_get(&app->asset_system.assets, handle);
  }

  if (asset->status == NYA_ASSET_STATUS_LOADED) return;

  // Load data
  NYA_Asset loaded       = { 0 };
  loaded.type            = type;
  loaded.handle          = handle;
  loaded.status          = NYA_ASSET_STATUS_LOADED;
  loaded.reference_count = 0;
  memset(&loaded.gpu_resource, 0, sizeof(loaded.gpu_resource));

#ifdef NYA_ASSET_BACKEND_BLOB
  _nya_asset_load_from_blob(&loaded);
#endif

#ifdef NYA_ASSET_BACKEND_FS
  loaded = _nya_asset_load_from_filesystem(handle, type);
#endif

  // Create GPU resource
  _nya_asset_create_gpu_resource(&loaded, params);

  nya_hmap_set(&app->asset_system.assets, handle, loaded);
}

#ifdef NYA_ASSET_BACKEND_BLOB
NYA_INTERNAL void _nya_asset_load_from_blob(NYA_Asset* asset) {
  if (asset->type == NYA_ASSET_TYPE_BUFFER_VERTEX || asset->type == NYA_ASSET_TYPE_BUFFER_INDEX || asset->type == NYA_ASSET_TYPE_BUFFER_UNIFORM ||
      asset->type == NYA_ASSET_TYPE_GRAPHICS_PIPELINE) {
    asset->data             = nullptr;
    asset->size             = 0;
    asset->source_timestamp = 0;
  } else {
    for (u64 i = 0; i < NYA_ASSET_BLOB_HEADER_COUNT; i++) {
      NYA_AssetBlobHeader header = NYA_ASSET_BLOB_HEADER[i];
      if (nya_string_equals(header.handle, asset->handle)) {
        asset->data             = (u8*)NYA_ASSET_BLOB + header.start;
        asset->size             = header.size;
        asset->source_timestamp = 0;
        break;
      }
    }
  }
}
#endif // NYA_ASSET_BACKEND_BLOB

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL void _nya_asset_unload_from_filesystem(NYA_Asset* asset) {
  nya_assert(asset);

  NYA_App*   app     = nya_app_get_instance();
  NYA_Arena* arena   = &app->asset_system.allocator;
  NYA_String content = {
    .arena    = arena,
    .items    = asset->data,
    .length   = asset->size,
    .capacity = asset->size,
  };
  nya_string_destroy(&content);

  asset->status = NYA_ASSET_STATUS_UNLOADED;
  asset->data   = nullptr;
  asset->size   = 0;
}
#endif // NYA_ASSET_BACKEND_FS

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE HELPER FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL void _nya_asset_create_gpu_resource(NYA_Asset* asset, NYA_AssetLoadParams params) {
  NYA_App*       app    = nya_app_get_instance();
  SDL_GPUDevice* device = app->render_system.gpu_device;

  switch (asset->type) {
    case NYA_ASSET_TYPE_SHADER_VERTEX:
    case NYA_ASSET_TYPE_SHADER_FRAGMENT:
    case NYA_ASSET_TYPE_SHADER_COMPUTE:  {
      SDL_GPUShaderCreateInfo create_info = {
        .code                 = asset->data,
        .code_size            = asset->size,
        .entrypoint           = "main",
        .format               = NYA_SHADER_COMPILE_FORMAT,
        .stage                = (asset->type == NYA_ASSET_TYPE_SHADER_VERTEX)   ? SDL_GPU_SHADERSTAGE_VERTEX
                              : (asset->type == NYA_ASSET_TYPE_SHADER_FRAGMENT) ? SDL_GPU_SHADERSTAGE_FRAGMENT
                                                                                : SDL_GPU_SHADERSTAGE_COMPUTE,
        .num_samplers         = params.shader.num_samplers,
        .num_storage_textures = params.shader.num_storage_textures,
        .num_storage_buffers  = params.shader.num_storage_buffers,
        .num_uniform_buffers  = params.shader.num_uniform_buffers,
      };
      asset->gpu_resource.shader = SDL_CreateGPUShader(device, &create_info);
      break;
    }
    case NYA_ASSET_TYPE_BUFFER_VERTEX:
    case NYA_ASSET_TYPE_BUFFER_INDEX:
    case NYA_ASSET_TYPE_BUFFER_UNIFORM: {
      SDL_GPUBufferCreateInfo create_info = {
        .size  = params.buffer.size,
        .usage = params.buffer.usage,
      };
      asset->gpu_resource.buffer = SDL_CreateGPUBuffer(device, &create_info);
      break;
    }
    case NYA_ASSET_TYPE_GRAPHICS_PIPELINE: {
      NYA_Asset* vertex_shader   = nya_asset_acquire(params.pipeline.vertex_shader_path);
      NYA_Asset* fragment_shader = nya_asset_acquire(params.pipeline.fragment_shader_path);
      if (!vertex_shader || !fragment_shader) { nya_panic("Shaders not loaded for pipeline %s", asset->handle); }
      SDL_GPUGraphicsPipelineCreateInfo create_info = params.pipeline.create_info;
      create_info.vertex_shader                     = vertex_shader->gpu_resource.shader;
      create_info.fragment_shader                   = fragment_shader->gpu_resource.shader;
      asset->gpu_resource.graphics_pipeline         = SDL_CreateGPUGraphicsPipeline(device, &create_info);
      break;
    }
    case NYA_ASSET_TYPE_TEXTURE: {
      // TODO: implement texture loading
      break;
    }
    default: break;
  }
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * GPU ASSET PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

void nya_asset_load_batch_begin(void) {
  NYA_App* app = nya_app_get_instance();
  nya_assert(!app->asset_system.batch_active, "Asset batch already active");
  app->asset_system.batch_active = true;
  nya_array_clear(&app->asset_system.batch_types);
  nya_array_clear(&app->asset_system.batch_requests);
}

void nya_asset_load_batch_add(NYA_AssetType type, NYA_AssetLoadParams params) {
  NYA_App* app = nya_app_get_instance();
  nya_assert(app->asset_system.batch_active, "Asset batch not active");
  nya_array_push_back(&app->asset_system.batch_types, (u32)type);
  nya_array_push_back(&app->asset_system.batch_requests, params);
}

void nya_asset_load_batch_submit(void) {
  NYA_App* app = nya_app_get_instance();
  nya_assert(app->asset_system.batch_active);

  for (u64 i = 0; i < app->asset_system.batch_requests.length; i++) {
    NYA_AssetType       type   = (NYA_AssetType)app->asset_system.batch_types.items[i];
    NYA_AssetLoadParams params = app->asset_system.batch_requests.items[i];
    nya_asset_load(type, params.handle, params);
  }

  app->asset_system.batch_active = false;
}

NYA_Asset* nya_asset_load_and_acquire(NYA_AssetType type, NYA_AssetHandle handle, NYA_AssetLoadParams params) {
  nya_asset_load(type, handle, params);
  return nya_asset_acquire(handle);
}

NYA_Asset* nya_asset_acquire(NYA_AssetHandle handle) {
  NYA_Asset* asset = nya_asset_get_struct(handle);
  if (!asset || asset->status != NYA_ASSET_STATUS_LOADED) return nullptr;
  atomic_fetch_add(&asset->reference_count, 1);
  return asset;
}

void nya_asset_release(NYA_AssetHandle handle) {
  NYA_Asset* asset = nya_asset_get_struct(handle);
  if (!asset) return;
  if (atomic_fetch_sub(&asset->reference_count, 1) == 1) {
    // Ref count reached 0, unload
    nya_asset_unload(handle);
  }
}

void nya_asset_unload(NYA_AssetHandle handle) {
  NYA_Asset* asset = nya_asset_get_struct(handle);
  if (!asset) return;
  if (asset->status == NYA_ASSET_STATUS_UNLOADED) return;
  // Destroy GPU resources
  NYA_App* app = nya_app_get_instance();
  switch (asset->type) {
    case NYA_ASSET_TYPE_TEXTURE:
      if (asset->gpu_resource.texture) SDL_ReleaseGPUTexture(app->render_system.gpu_device, asset->gpu_resource.texture);
      break;
    case NYA_ASSET_TYPE_SHADER_VERTEX:
    case NYA_ASSET_TYPE_SHADER_FRAGMENT:
      if (asset->gpu_resource.shader) SDL_ReleaseGPUShader(app->render_system.gpu_device, asset->gpu_resource.shader);
      break;
    case NYA_ASSET_TYPE_BUFFER_VERTEX:
    case NYA_ASSET_TYPE_BUFFER_INDEX:
    case NYA_ASSET_TYPE_BUFFER_UNIFORM:
      if (asset->gpu_resource.buffer) SDL_ReleaseGPUBuffer(app->render_system.gpu_device, asset->gpu_resource.buffer);
      break;
    case NYA_ASSET_TYPE_GRAPHICS_PIPELINE:
      if (asset->gpu_resource.graphics_pipeline) SDL_ReleaseGPUGraphicsPipeline(app->render_system.gpu_device, asset->gpu_resource.graphics_pipeline);
      break;
    default: break;
  }

    // Unload from backend
#ifdef NYA_ASSET_BACKEND_FS
  if (asset->data) _nya_asset_unload_from_filesystem(asset);
#endif

  asset->status = NYA_ASSET_STATUS_UNLOADED;
  asset->data   = nullptr;
  asset->size   = 0;
  memset(&asset->gpu_resource, 0, sizeof(asset->gpu_resource));
  asset->reference_count = 0;
}
