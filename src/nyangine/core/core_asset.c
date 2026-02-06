#include "SDL3/SDL_gpu.h"

#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLRARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if defined(NYA_ASSET_BACKEND_FS) && defined(NYA_ASSET_BACKEND_BLOB)
#error "Only one asset backend can be defined at a time."
#elif !defined(NYA_ASSET_BACKEND_FS) && !defined(NYA_ASSET_BACKEND_BLOB)
#error "An asset backend must be defined."
#endif

#ifdef NYA_ASSET_BACKEND_BLOB
#include "../../../assets/assets.c"

NYA_INTERNAL void _nya_asset_load_raw_from_blob(NYA_AssetHandle path, OUT NYA_Asset* out_asset);
#define _nya_asset_load_raw_from_backend   _nya_asset_load_raw_from_blob
#define _nya_asset_unload_raw_from_backend nya_unused
#endif // NYA_ASSET_BACKEND_BLOB

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL void _nya_asset_load_raw_from_filesystem(NYA_AssetHandle path, OUT NYA_Asset* out_asset);
NYA_INTERNAL void _nya_asset_unload_raw_from_filesystem(NYA_Asset* asset);
#define _nya_asset_load_raw_from_backend   _nya_asset_load_raw_from_filesystem
#define _nya_asset_unload_raw_from_backend _nya_asset_unload_raw_from_filesystem

NYA_INTERNAL b8 _nya_asset_get_modification_time(NYA_Asset* asset, OUT u64* out_modification_time);

void _nya_asset_reload_process(NYA_Event* event);
#endif // NYA_ASSET_BACKEND_FS

void _nya_asset_loading_process(NYA_Event* event);
void _nya_asset_unloading_process(NYA_Event* event);

NYA_INTERNAL NYA_AssetHandle _nya_asset_pick_correct_compiled_shader(NYA_AssetHandle source_shader, OUT SDL_GPUShaderFormat* out_format);

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

  app->asset_system.assets          = nya_hmap_create(app->asset_system.allocator, NYA_AssetHandle, NYA_Asset);
  app->asset_system.loading_queue   = nya_array_create(app->asset_system.allocator, NYA_AssetLoadParameters);
  app->asset_system.unloading_queue = nya_array_create(app->asset_system.allocator, NYA_AssetHandle);

  nya_event_hook_register((NYA_EventHook){
      .hook_type  = NYA_EVENT_HOOK_TYPE_IMMEDIATE,
      .event_type = NYA_EVENT_FRAME_ENDED,
      .fn         = nya_callback(_nya_asset_unloading_process),
  });

  nya_event_hook_register((NYA_EventHook){
      .hook_type  = NYA_EVENT_HOOK_TYPE_IMMEDIATE,
      .event_type = NYA_EVENT_FRAME_ENDED,
      .fn         = nya_callback(_nya_asset_loading_process),
  });

#ifdef NYA_ASSET_BACKEND_FS
  app->asset_system.reload_queue = nya_array_create(app->asset_system.allocator, NYA_AssetHandle);

  nya_event_hook_register((NYA_EventHook){
      .hook_type  = NYA_EVENT_HOOK_TYPE_IMMEDIATE,
      .event_type = NYA_EVENT_FRAME_ENDED,
      .fn         = nya_callback(_nya_asset_reload_process),
  });
#endif // NYA_ASSET_BACKEND_FS

  nya_info("Asset system initialized.");
}

void nya_system_asset_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  // submit all assets to be unloaded
  nya_hmap_foreach_value (app->asset_system.assets, asset) {
    if (asset && asset->status != NYA_ASSET_STATUS_UNLOADED) { /**/
      nya_array_push_back(app->asset_system.unloading_queue, asset->handle);
    }
  }
  _nya_asset_unloading_process(nullptr); // process unloading immediately

  nya_array_destroy(app->asset_system.loading_queue);
  nya_array_destroy(app->asset_system.unloading_queue);

#ifdef NYA_ASSET_BACKEND_FS
  nya_array_destroy(app->asset_system.reload_queue);
#endif // NYA_ASSET_BACKEND_FS

  nya_hmap_destroy(app->asset_system.assets);

  nya_arena_destroy(app->asset_system.allocator);

  nya_info("Asset system deinitialized.");
}

/*
 * ─────────────────────────────────────────────────────────
 * ASSET FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_Asset* nya_asset_get(NYA_AssetHandle handle) {
  nya_assert(handle != nullptr);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;
  NYA_Asset*       asset  = nya_hmap_get(system->assets, handle);

#ifdef NYA_ASSET_BACKEND_FS
  if (asset != nullptr && asset->status == NYA_ASSET_STATUS_LOADED) {
    u64 file_modification_time = 0;
    _nya_asset_get_modification_time(asset, &file_modification_time);

    if (file_modification_time > asset->source_modification_time) { /**/
      if (nya_array_contains(system->reload_queue, asset->handle)) return asset;

      nya_array_push_back(system->reload_queue, asset->handle);
      nya_info("Asset marked for reload due to modification: %s", asset->handle);
    }
  }
#endif // NYA_ASSET_BACKEND_FS

  return asset;
}

void nya_asset_acquire(NYA_AssetHandle handle) {
  nya_assert(handle != nullptr);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  NYA_Asset* asset = nya_hmap_get(system->assets, handle);
  nya_assert(asset != nullptr, "Cannot acquire unloaed asset: %s", handle);

  atomic_fetch_add(&asset->reference_count, 1);
}

void nya_asset_release(NYA_AssetHandle handle) {
  nya_assert(handle != nullptr);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  NYA_Asset* asset = nya_hmap_get(system->assets, handle);
  nya_assert(asset != nullptr, "Cannot release unloaed asset: %s", handle);

  u64 prev_reference_count = atomic_fetch_sub(&asset->reference_count, 1);

  if (prev_reference_count == 1) nya_asset_unload(asset);
}

void nya_asset_load(NYA_AssetLoadParameters parameters) {
  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  NYA_Asset* asset = nya_hmap_get(system->assets, parameters.handle);
  if (asset != nullptr && asset->status != NYA_ASSET_STATUS_UNLOADED) return;

  NYA_Asset new_asset = (NYA_Asset){
    .handle          = parameters.handle,
    .status          = NYA_ASSET_STATUS_LOADING,
    .load_parameters = parameters,
    .reference_count = 0,
  };

  nya_hmap_set(system->assets, parameters.handle, new_asset);
  nya_array_push_back(system->loading_queue, parameters);

  nya_info("Queuing asset for loading: %s", parameters.handle);
}

void nya_asset_unload(NYA_Asset* asset) {
  nya_assert(asset != nullptr);
  nya_assert(asset->reference_count == 0, "Cannot unload asset with non-zero reference count: %s", asset->handle);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  nya_array_push_back(system->unloading_queue, asset->handle);

  nya_info("Queuing asset for unloading: %s", asset->handle);
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#ifdef NYA_ASSET_BACKEND_BLOB
NYA_INTERNAL void _nya_asset_load_raw_from_blob(NYA_AssetHandle path, OUT NYA_Asset* out_asset) {
  nya_assert(path != nullptr);

  for (u64 asset_header_index = 0; asset_header_index < NYA_ASSET_BLOB_HEADER_COUNT; asset_header_index++) {
    NYA_AssetBlobHeader asset_header = NYA_ASSET_BLOB_HEADER[asset_header_index];
    if (!nya_string_equals(asset_header.path, path)) continue;

    out_asset->as_text.data = (u8*)NYA_ASSET_BLOB + asset_header.start;
    out_asset->as_text.size = asset_header.size;

    return;
  }

  nya_panic("Asset not found in blob: %s", path);
  nya_unreachable();
}
#endif // NYA_ASSET_BACKEND_BLOB

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL void _nya_asset_load_raw_from_filesystem(NYA_CString path, OUT NYA_Asset* out_asset) {
  nya_assert(path != nullptr);

  if (!nya_filesystem_exists(path)) {
    nya_panic("Asset not found in filesystem: %s", path);
    nya_unreachable();
  }

  NYA_App*   app   = nya_app_get_instance();
  NYA_Arena* arena = app->asset_system.allocator;

  b8 ok;

  NYA_String* content = nya_string_create(arena);
  ok                  = nya_file_read(path, content);
  nya_string_shrink_to_fit(content);
  nya_assert(ok);
  u8* data = content->items;
  u64 size = content->length;
  nya_arena_free(content->arena, content, sizeof(NYA_String));

  u64 modification_time = 0;
  ok                    = nya_filesystem_last_modified(path, &modification_time);
  nya_assert(ok);

  out_asset->as_text.data             = data;
  out_asset->as_text.size             = size;
  out_asset->source_modification_time = modification_time;
}
#endif // NYA_ASSET_BACKEND_FS

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL void _nya_asset_unload_raw_from_filesystem(NYA_Asset* asset) {
  nya_assert(asset != nullptr);

  NYA_App*   app   = nya_app_get_instance();
  NYA_Arena* arena = app->asset_system.allocator;

  nya_arena_free(arena, asset->as_text.data, asset->as_text.size);

  asset->as_text.data = nullptr;
  asset->as_text.size = 0;
}
#endif // NYA_ASSET_BACKEND_FS

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL b8 _nya_asset_get_modification_time(NYA_Asset* asset, OUT u64* out_modification_time) {
  b8 ok;
  switch (asset->type) {
    case NYA_ASSET_TYPE_TEXT: {
      ok = nya_filesystem_last_modified(asset->handle, out_modification_time);
    } break;
    case NYA_ASSET_TYPE_SHADER_VERTEX:
    case NYA_ASSET_TYPE_SHADER_FRAGMENT: {
      ok = nya_filesystem_last_modified(asset->as_shader.compiled_handle, out_modification_time);
    } break;

    default: {
      *out_modification_time = 0;
      return true;
    } break;
  }

  if (!ok) nya_warn("Loaded asset missing from filesystem: %s", asset->handle);
  return ok;
}
#endif // NYA_ASSET_BACKEND_FS

void _nya_asset_loading_process(NYA_Event* event) {
  nya_unused(event);

  NYA_AssetSystem*  system        = &nya_app_get_instance()->asset_system;
  NYA_RenderSystem* render_system = &nya_app_get_instance()->render_system;

  nya_array_foreach (system->loading_queue, parameters) {
    nya_info("Loading asset: %s", parameters->handle);

    NYA_Asset* asset = nya_hmap_get(system->assets, parameters->handle);
    nya_assert(asset != nullptr);

    switch (parameters->type) {
      case NYA_ASSET_TYPE_TEXT: {
        _nya_asset_load_raw_from_backend(parameters->handle, asset);
        asset->type   = NYA_ASSET_TYPE_TEXT;
        asset->status = NYA_ASSET_STATUS_LOADED;
      } break;

      case NYA_ASSET_TYPE_SHADER_VERTEX:
      case NYA_ASSET_TYPE_SHADER_FRAGMENT: {
        SDL_GPUShaderFormat format;
        NYA_AssetHandle     compiled_shader_handle = _nya_asset_pick_correct_compiled_shader(parameters->handle, &format);
        _nya_asset_load_raw_from_backend(compiled_shader_handle, asset);

        SDL_GPUShaderStage stage;
        if (parameters->type == NYA_ASSET_TYPE_SHADER_VERTEX) {
          stage = SDL_GPU_SHADERSTAGE_VERTEX;
        } else {
          stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
        }

        SDL_GPUShader* shader = SDL_CreateGPUShader(
            render_system->gpu_device,
            &(SDL_GPUShaderCreateInfo){
                .code_size            = asset->as_text.size,
                .code                 = asset->as_text.data,
                .entrypoint           = "main",
                .format               = format,
                .stage                = stage,
                .num_samplers         = parameters->as_shader.num_samplers,
                .num_storage_textures = parameters->as_shader.num_storage_textures,
                .num_storage_buffers  = parameters->as_shader.num_storage_buffers,
                .num_uniform_buffers  = parameters->as_shader.num_uniform_buffers,
            }
        );
        nya_assert(shader != nullptr, "Failed to compile shader: %s", parameters->handle);
        _nya_asset_unload_raw_from_backend(asset); // unload raw text code

        asset->type                      = NYA_ASSET_TYPE_SHADER_VERTEX;
        asset->status                    = NYA_ASSET_STATUS_LOADED;
        asset->as_shader.compiled_handle = compiled_shader_handle;
        asset->as_shader.format          = format;
        asset->as_shader.shader          = shader;
      } break;

      default: {
        nya_panic("Unsupported asset type for loading: %d", parameters->type);
      } break;
    }
  }

  nya_array_clear(system->loading_queue);
}

void _nya_asset_unloading_process(NYA_Event* event) {
  nya_unused(event);

  NYA_AssetSystem*  system        = &nya_app_get_instance()->asset_system;
  NYA_RenderSystem* render_system = &nya_app_get_instance()->render_system;

  nya_array_foreach (system->unloading_queue, handle) {
    nya_info("Unloading asset: %s", *handle);

    NYA_Asset* asset = nya_hmap_get(system->assets, *handle);
    nya_assert(asset != nullptr);

    switch (asset->type) {
      case NYA_ASSET_TYPE_TEXT: {
        _nya_asset_unload_raw_from_backend(asset);
        asset->status = NYA_ASSET_STATUS_UNLOADED;
      } break;

      case NYA_ASSET_TYPE_SHADER_VERTEX:
      case NYA_ASSET_TYPE_SHADER_FRAGMENT: {
        SDL_ReleaseGPUShader(render_system->gpu_device, asset->as_shader.shader);
        nya_arena_free(system->allocator, (void*)asset->as_shader.compiled_handle, strlen(asset->as_shader.compiled_handle));
        asset->status = NYA_ASSET_STATUS_UNLOADED;
      } break;

      default: {
        nya_panic("Unsupported asset type for unloading: %d", asset->type);
      } break;
    }
  }

  nya_array_clear(system->unloading_queue);
}

#ifdef NYA_ASSET_BACKEND_FS
void _nya_asset_reload_process(NYA_Event* event) {
  nya_unused(event);
  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  NYA_AssetHandleArray* postponed = nya_array_create(system->allocator, NYA_AssetHandle);

  nya_array_foreach (system->reload_queue, handle) {
    NYA_Asset* asset = nya_hmap_get(system->assets, *handle);
    nya_assert(asset != nullptr);

    u64 file_modification_time = 0;
    _nya_asset_get_modification_time(asset, &file_modification_time);

    if (file_modification_time != asset->source_modification_time) {
      nya_info("Postponing reload of asset (still written to): %s", *handle);
      asset->source_modification_time = file_modification_time;
      nya_array_push_back(postponed, *handle);
    } else {
      nya_info("Reloading asset: %s", *handle);
      nya_array_push_back(system->unloading_queue, *handle);
      nya_array_push_back(system->loading_queue, asset->load_parameters);
    }
  }

  nya_array_clear(system->reload_queue);

  nya_array_foreach (postponed, handle) nya_array_push_back(system->reload_queue, *handle);
  nya_array_destroy(postponed);
}
#endif // NYA_ASSET_BACKEND_FS

NYA_INTERNAL NYA_AssetHandle _nya_asset_pick_correct_compiled_shader(NYA_AssetHandle source_shader, OUT SDL_GPUShaderFormat* out_format) {
  nya_assert(nya_string_contains(source_shader, "/shaders/source/"));

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  NYA_String* compiled_shader_path = nya_string_from(system->allocator, source_shader);
  nya_string_replace(compiled_shader_path, "/shaders/source/", "/shaders/compiled/");
  nya_string_strip_suffix(compiled_shader_path, ".hlsl");

  switch (NYA_OS_CURRENT) {
    case NYA_OS_WINDOWS: {
      nya_string_extend(compiled_shader_path, ".dxil");
      *out_format = SDL_GPU_SHADERFORMAT_DXIL;
    } break;

    case NYA_OS_LINUX: {
      nya_string_extend(compiled_shader_path, ".spv");
      *out_format = SDL_GPU_SHADERFORMAT_SPIRV;
    } break;

    case NYA_OS_MAC: {
      nya_string_extend(compiled_shader_path, ".msl");
      *out_format = SDL_GPU_SHADERFORMAT_MSL;
    } break;

    default: {
      nya_panic("Unsupported OS for picking compiled shader: %d", NYA_OS_CURRENT);
    } break;
  }

  nya_string_extend(compiled_shader_path, "\0");

  NYA_CString handle = nya_arena_alloc(system->allocator, compiled_shader_path->length);
  nya_memcpy(handle, compiled_shader_path->items, compiled_shader_path->length);

  nya_string_destroy(compiled_shader_path);

  return handle;
}
