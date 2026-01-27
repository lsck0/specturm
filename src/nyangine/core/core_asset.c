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

NYA_INTERNAL NYA_Asset _nya_asset_load_raw_from_blob(NYA_AssetHandle path);
#endif // NYA_ASSET_BACKEND_BLOB

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL NYA_Asset _nya_asset_load_from_filesystem(NYA_AssetHandle path);
NYA_INTERNAL void      _nya_asset_unload_from_filesystem(NYA_Asset* asset);
#endif // NYA_ASSET_BACKEND_FS

NYA_INTERNAL void _nya_asset_loading_process(NYA_Event* event);
NYA_INTERNAL void _nya_asset_unloading_process(NYA_Event* event);

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL void _nya_asset_reload_process(NYA_Event* event);
#endif // NYA_ASSET_BACKEND_FS

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

  app->asset_system.assets          = nya_hmap_create(&app->asset_system.allocator, NYA_AssetHandle, NYA_Asset);
  app->asset_system.loading_queue   = nya_array_create(&app->asset_system.allocator, NYA_AssetLoadParameters);
  app->asset_system.unloading_queue = nya_array_create(&app->asset_system.allocator, NYA_AssetHandle);

  nya_event_hook_register((NYA_EventHook){
      .hook_type  = NYA_EVENT_HOOK_TYPE_IMMEDIATE,
      .event_type = NYA_EVENT_FRAME_ENDED,
      .fn         = _nya_asset_unloading_process,
  });

  nya_event_hook_register((NYA_EventHook){
      .hook_type  = NYA_EVENT_HOOK_TYPE_IMMEDIATE,
      .event_type = NYA_EVENT_FRAME_ENDED,
      .fn         = _nya_asset_loading_process,
  });

#ifdef NYA_ASSET_BACKEND_FS
  app->asset_system.reload_queue = nya_array_create(&app->asset_system.allocator, NYA_AssetHandle);

  nya_event_hook_register((NYA_EventHook){
      .hook_type  = NYA_EVENT_HOOK_TYPE_IMMEDIATE,
      .event_type = NYA_EVENT_FRAME_ENDED,
      .fn         = _nya_asset_reload_process,
  });
#endif // NYA_ASSET_BACKEND_FS

  nya_info("Asset system initialized.");
}

void nya_system_asset_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  nya_array_destroy(&app->asset_system.loading_queue);
  nya_array_destroy(&app->asset_system.unloading_queue);

#ifdef NYA_ASSET_BACKEND_FS
  nya_array_destroy(&app->asset_system.reload_queue);
#endif // NYA_ASSET_BACKEND_FS

  nya_hmap_destroy(&app->asset_system.assets);

  nya_arena_destroy(&app->asset_system.allocator);

  nya_info("Asset system deinitialized.");
}

/*
 * ─────────────────────────────────────────────────────────
 * ASSET FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_asset_generate_manifest(NYA_ConstCString asset_directory, NYA_ConstCString output_file) {
  nya_assert(asset_directory != nullptr);
  nya_assert(output_file != nullptr);

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
    if (nya_string_is_empty(file)) continue;

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
  nya_assert(asset_directory != nullptr);
  nya_assert(output_file != nullptr);

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
    if (nya_string_is_empty(file)) continue;

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

NYA_Asset* nya_asset_get(NYA_AssetHandle handle) {
  nya_assert(handle != nullptr);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;
  NYA_Asset*       asset  = nya_hmap_get(&system->assets, handle);

#ifdef NYA_ASSET_BACKEND_FS
  if (asset != nullptr && asset->status == NYA_ASSET_STATUS_LOADED) {
    u64 file_modification_time = 0;
    b8  ok                     = nya_filesystem_last_modified(asset->handle, &file_modification_time);
    if (!ok) {
      nya_warn("Loaded asset missing from filesystem: %s", asset->handle);
      return asset;
    }

    if (file_modification_time > asset->source_modification_time) { /**/
      if (nya_array_contains(&system->reload_queue, asset->handle)) return asset;

      nya_array_push_back(&system->reload_queue, asset->handle);
      nya_info("Asset marked for reload due to modification: %s", asset->handle);
    }
  }
#endif // NYA_ASSET_BACKEND_FS

  return asset;
}

void nya_asset_acquire(NYA_AssetHandle handle) {
  nya_assert(handle != nullptr);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  NYA_Asset* asset = nya_hmap_get(&system->assets, handle);
  nya_assert(asset != nullptr, "Cannot acquire unloaed asset: %s", handle);

  atomic_fetch_add(&asset->reference_count, 1);
}

void nya_asset_release(NYA_AssetHandle handle) {
  nya_assert(handle != nullptr);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  NYA_Asset* asset = nya_hmap_get(&system->assets, handle);
  nya_assert(asset != nullptr, "Cannot release unloaed asset: %s", handle);

  u64 prev_reference_count = atomic_fetch_sub(&asset->reference_count, 1);

  if (prev_reference_count == 1) nya_asset_unload(asset);
}

void nya_asset_load(NYA_AssetLoadParameters parameters) {
  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  NYA_Asset* asset = nya_hmap_get(&system->assets, parameters.handle);
  if (asset != nullptr && asset->status != NYA_ASSET_STATUS_UNLOADED) return;

  NYA_Asset new_asset = (NYA_Asset){
    .handle          = parameters.handle,
    .status          = NYA_ASSET_STATUS_LOADING,
    .load_parameters = parameters,
    .reference_count = 0,
  };

  nya_hmap_set(&system->assets, parameters.handle, new_asset);
  nya_array_push_back(&system->loading_queue, parameters);

  nya_info("Queuing asset for loading: %s", parameters.handle);
}

void nya_asset_unload(NYA_Asset* asset) {
  nya_assert(asset != nullptr);
  nya_assert(asset->reference_count == 0, "Cannot unload asset with non-zero reference count: %s", asset->handle);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  nya_array_push_back(&system->unloading_queue, asset->handle);

  nya_info("Queuing asset for unloading: %s", asset->handle);
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#ifdef NYA_ASSET_BACKEND_BLOB
NYA_INTERNAL NYA_Asset _nya_asset_load_raw_from_blob(NYA_AssetHandle path) {
  nya_assert(path != nullptr);

  for (u64 asset_header_index = 0; asset_header_index < NYA_ASSET_BLOB_HEADER_COUNT; asset_header_index++) {
    NYA_AssetBlobHeader asset_header = NYA_ASSET_BLOB_HEADER[asset_header_index];
    if (!nya_string_equals(asset_header.path, path)) continue;

    return (NYA_Asset){
      .as_text.data = (u8*)NYA_ASSET_BLOB + asset_header.start,
      .as_text.size = asset_header.size,
    };
  }

  nya_panic("Asset not found in blob: %s", path);
  nya_unreachable();
}
#endif // NYA_ASSET_BACKEND_BLOB

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL NYA_Asset _nya_asset_load_raw_from_filesystem(NYA_CString path) {
  nya_assert(path != nullptr);

  if (!nya_filesystem_exists(path)) {
    nya_panic("Asset not found in filesystem: %s", path);
    nya_unreachable();
  }

  NYA_App*   app   = nya_app_get_instance();
  NYA_Arena* arena = &app->asset_system.allocator;

  b8 ok;

  NYA_String content = nya_string_create(arena);
  ok                 = nya_file_read(path, &content);
  nya_string_shrink_to_fit(&content);
  nya_assert(ok);

  u64 modification_time = 0;
  ok                    = nya_filesystem_last_modified(path, &modification_time);
  nya_assert(ok);

  return (NYA_Asset){
    .as_text.data             = content.items,
    .as_text.size             = content.length,
    .source_modification_time = modification_time,
  };
}
#endif // NYA_ASSET_BACKEND_FS

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL void _nya_asset_unload_raw_from_filesystem(NYA_Asset* asset) {
  nya_assert(asset != nullptr);

  NYA_App*   app     = nya_app_get_instance();
  NYA_Arena* arena   = &app->asset_system.allocator;
  NYA_String content = {
    .arena    = arena,
    .items    = asset->as_text.data,
    .length   = asset->as_text.size,
    .capacity = asset->as_text.size,
  };
  nya_string_destroy(&content);

  asset->as_text.data = nullptr;
  asset->as_text.size = 0;
}
#endif // NYA_ASSET_BACKEND_FS

void _nya_asset_loading_process(NYA_Event* event) {
  nya_unused(event);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  nya_array_foreach (&system->loading_queue, parameters) {
    nya_info("Loading asset: %s", parameters->handle);

    NYA_Asset* asset = nya_hmap_get(&system->assets, parameters->handle);
    nya_assert(asset != nullptr);

    NYA_Asset data;
#ifdef NYA_ASSET_BACKEND_BLOB
    data = _nya_asset_load_raw_from_blob(parameters->handle);
#endif // NYA_ASSET_BACKEND_BLOB
#ifdef NYA_ASSET_BACKEND_FS
    data = _nya_asset_load_raw_from_filesystem(parameters->handle);
#endif // NYA_ASSET_BACKEND_FS

    asset->as_text.data = data.as_text.data;
    asset->as_text.size = data.as_text.size;

#ifdef NYA_ASSET_BACKEND_FS
    asset->source_modification_time = data.source_modification_time;
#endif // NYA_ASSET_BACKEND_FS

    switch (parameters->type) {
      case NYA_ASSET_TYPE_TEXT: {
        asset->type   = NYA_ASSET_TYPE_TEXT;
        asset->status = NYA_ASSET_STATUS_LOADED;
      } break;

      default: {
        nya_panic("Unsupported asset type for loading: %d", parameters->type);
      } break;
    }
  }

  nya_array_clear(&system->loading_queue);
}

void _nya_asset_unloading_process(NYA_Event* event) {
  nya_unused(event);

  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  nya_array_foreach (&system->unloading_queue, handle) {
    nya_info("Unloading asset: %s", *handle);

    NYA_Asset* asset = nya_hmap_get(&system->assets, *handle);
    nya_assert(asset != nullptr);

    switch (asset->type) {
      case NYA_ASSET_TYPE_TEXT: {
#ifdef NYA_ASSET_BACKEND_FS
        _nya_asset_unload_raw_from_filesystem(asset);
#endif // NYA_ASSET_BACKEND_FS
        asset->status = NYA_ASSET_STATUS_UNLOADED;
      } break;

      default: {
        nya_panic("Unsupported asset type for unloading: %d", asset->type);
      } break;
    }
  }

  nya_array_clear(&system->unloading_queue);
}

#ifdef NYA_ASSET_BACKEND_FS
void _nya_asset_reload_process(NYA_Event* event) {
  nya_unused(event);
  NYA_AssetSystem* system = &nya_app_get_instance()->asset_system;

  nya_array_foreach (&system->reload_queue, handle) {
    nya_info("Reloading asset: %s", *handle);

    NYA_Asset* asset = nya_hmap_get(&system->assets, *handle);
    nya_assert(asset != nullptr);

    nya_array_push_back(&system->unloading_queue, *handle);
    nya_array_push_back(&system->loading_queue, asset->load_parameters);
  }

  nya_array_clear(&system->reload_queue);
}
#endif // NYA_ASSET_BACKEND_FS
