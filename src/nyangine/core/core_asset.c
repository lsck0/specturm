#include "nyangine/base/base_hset.h"
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

NYA_INTERNAL NYA_Asset _nya_asset_load_from_blob(NYA_CString path);
#endif // NYA_ASSET_BACKEND_BLOB

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL NYA_Asset _nya_asset_load_from_filesystem(NYA_CString path);
NYA_INTERNAL void      _nya_asset_unload_from_filesystem(NYA_Asset* asset);
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

  app->asset_system.assets         = nya_hmap_create(&app->asset_system.allocator, NYA_CString, NYA_Asset);
  app->asset_system.changed_assets = nya_array_create(&app->asset_system.allocator, NYA_CString);
}

void nya_system_asset_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  nya_array_destroy(&app->asset_system.changed_assets);
  nya_hmap_destroy(&app->asset_system.assets);

  nya_arena_destroy(&app->asset_system.allocator);
}

void nya_system_asset_handle_event(NYA_Event* event) {
  nya_assert(event);

  NYA_App* app = nya_app_get_instance();

  if (event->type == NYA_EVENT_NEW_TICK) {
    nya_array_foreach (&app->asset_system.changed_assets, asset_path) {
      NYA_Asset* asset = nya_asset_get(*asset_path);
      nya_asset_unload(asset);
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

NYA_Asset* nya_asset_get(NYA_CString path) {
  nya_assert(path);

  NYA_App*   app   = nya_app_get_instance();
  NYA_Asset* asset = nya_hmap_get(&app->asset_system.assets, path);
  if (asset) {

#ifdef NYA_ASSET_BACKEND_FS
    if (asset->status == NYA_ASSET_STATUS_LOADED && !nya_array_contains(&app->asset_system.changed_assets, asset->path)) {
      u64 modification_time = 0;
      b8  ok                = nya_filesystem_last_modified(path, &modification_time);
      nya_assert(ok);

      if (modification_time != asset->path_modification_time) nya_array_push_back(&app->asset_system.changed_assets, asset->path);
    }
#endif // NYA_ASSET_BACKEND_FS

    return asset;
  }

  NYA_Asset new_asset = {
    .status = NYA_ASSET_STATUS_UNLOADED,
    .path   = path,
  };
  nya_hmap_set(&app->asset_system.assets, path, new_asset);
  asset = nya_hmap_get(&app->asset_system.assets, path);
  return asset;
}

NYA_Asset* nya_asset_load_and_get(NYA_CString path) {
  nya_assert(path);

  NYA_Asset* asset = nya_asset_get(path);
  if (asset->status == NYA_ASSET_STATUS_LOADED) return asset;

  nya_asset_load_blocking(path);
  return asset;
}

void nya_asset_load_blocking(NYA_CString path) {
  nya_assert(path);

  NYA_App*   app   = nya_app_get_instance();
  NYA_Asset* asset = nya_asset_get(path);
  if (asset->status == NYA_ASSET_STATUS_LOADED) return;

  if (asset->status == NYA_ASSET_STATUS_LOADING) {
    nya_panic("Do not mix synchronous and asynchronous loading of the same asset: %s", path);
    nya_unreachable();
  }

  if (asset->status != NYA_ASSET_STATUS_UNLOADED) {
    nya_panic("Asset is in invalid state for loading: %s", path);
    nya_unreachable();
  }

  NYA_Asset loaded_asset;

#ifdef NYA_ASSET_BACKEND_BLOB
  loaded_asset = _nya_asset_load_from_blob(path);
#endif // NYA_ASSET_BACKEND_BLOB

#ifdef NYA_ASSET_BACKEND_FS
  loaded_asset = _nya_asset_load_from_filesystem(path);
#endif // NYA_ASSET_BACKEND_FS

  nya_hmap_set(&app->asset_system.assets, path, loaded_asset);
}

void nya_asset_load_async(NYA_CString path) {
  nya_unused(path);
  nya_unimplemented();
}

void nya_asset_unload(NYA_Asset* asset) {
  nya_assert(asset);

  if (asset->status == NYA_ASSET_STATUS_UNLOADED) return;

#ifdef NYA_ASSET_BACKEND_FS
  _nya_asset_unload_from_filesystem(asset);
#endif // NYA_ASSET_BACKEND_FS
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#ifdef NYA_ASSET_BACKEND_BLOB
NYA_INTERNAL NYA_Asset _nya_asset_load_from_blob(NYA_CString path) {
  nya_assert(path);

  for (u64 asset_header_index = 0; asset_header_index < NYA_ASSET_BLOB_HEADER_COUNT; asset_header_index++) {
    NYA_AssetBlobHeader asset_header = NYA_ASSET_BLOB_HEADER[asset_header_index];
    if (!nya_string_equals(asset_header.path, path)) continue;

    return (NYA_Asset){
      .path                   = path,
      .status                 = NYA_ASSET_STATUS_LOADED,
      .data                   = (u8*)NYA_ASSET_BLOB + asset_header.start,
      .size                   = asset_header.size,
      .path_modification_time = 0,
    };
  }

  nya_panic("Asset not found in blob: %s", path);
  nya_unreachable();
}
#endif // NYA_ASSET_BACKEND_BLOB

#ifdef NYA_ASSET_BACKEND_FS
NYA_INTERNAL NYA_Asset _nya_asset_load_from_filesystem(NYA_CString path) {
  nya_assert(path);

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
    .path                   = path,
    .status                 = NYA_ASSET_STATUS_LOADED,
    .data                   = content.items,
    .size                   = content.length,
    .path_modification_time = modification_time,
  };
}
#endif // NYA_ASSET_BACKEND_FS

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
