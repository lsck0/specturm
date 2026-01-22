#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLRARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if !NYA_IS_DEBUG
#include "../../../assets/assets.c"

NYA_EXTERN const u64                 NYA_ASSET_BLOB_HEADER_COUNT;
NYA_EXTERN const NYA_AssetBlobHeader NYA_ASSET_BLOB_HEADER[];
NYA_EXTERN const u8                  NYA_ASSET_BLOB[];
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
}

void nya_system_asset_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  nya_arena_destroy(&app->asset_system.allocator);
}

/*
 * ─────────────────────────────────────────────────────────
 * ASSET FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_asset_generate_embedding(NYA_ConstCString asset_directory, NYA_ConstCString output_file) {
  nya_assert(asset_directory);

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
          "-not", "-name", ".keep",
      },
  };
  nya_command_run(&find_assets_command);
  NYA_StringArray files = nya_string_split_lines(arena, &find_assets_command.stdout_content);
  nya_string_extend(&result, "/* THIS FILE IS GENERATED. DO NYAT TOUCH. */\n\n");
  nya_string_extend(&result, "#include \"nyangine/nyangine.h\"\n\n");
  header_count_string = nya_string_sprintf(arena, "const u64 NYA_ASSET_BLOB_HEADER_COUNT = " FMTu64 ";\n", files.length);
  nya_string_extend(&header_string, "const NYA_AssetBlobHeader NYA_ASSET_BLOB_HEADER[] = {\n");
  nya_string_extend(&blob_string, "const u8 NYA_ASSET_BLOB[] = {\n");

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
