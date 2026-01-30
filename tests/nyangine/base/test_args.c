#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  NYA_Arena* arena = nya_arena_create(.name = "test_args");

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: basic flag parameter
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_ArgParameter flag = {
      .kind        = NYA_ARG_PARAMETER_KIND_FLAG,
      .value.type  = NYA_TYPE_BOOL,
      .name        = "verbose",
      .description = "Enable verbose output",
    };

    NYA_ArgCommand root = {
      .is_root    = true,
      .parameters = { &flag },
    };

    NYA_ArgParser parser = {
      .name         = "test_app",
      .root_command = &root,
    };

    const char*     argv[] = { "app", "--verbose" };
    NYA_ArgCommand* cmd    = nya_args_parse_argv(&parser, 2, (NYA_CString*)argv);
    nya_assert(cmd != nullptr);
    nya_assert(flag.value.as_bool == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: flag with explicit true value
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_ArgParameter flag = {
      .kind       = NYA_ARG_PARAMETER_KIND_FLAG,
      .value.type = NYA_TYPE_BOOL,
      .name       = "debug",
    };

    NYA_ArgCommand root = {
      .is_root    = true,
      .parameters = { &flag },
    };

    NYA_ArgParser parser = {
      .name         = "test_app",
      .root_command = &root,
    };

    const char*     argv[] = { "app", "--debug", "true" };
    NYA_ArgCommand* cmd    = nya_args_parse_argv(&parser, 3, (NYA_CString*)argv);
    nya_assert(cmd != nullptr);
    nya_assert(flag.value.as_bool == true);

    const char* argv2[] = { "app", "--debug", "false" };
    cmd                 = nya_args_parse_argv(&parser, 3, (NYA_CString*)argv2);
    nya_assert(cmd != nullptr);
    nya_assert(flag.value.as_bool == false);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: integer flag parameter
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_ArgParameter count = {
      .kind       = NYA_ARG_PARAMETER_KIND_FLAG,
      .value.type = NYA_TYPE_INTEGER,
      .name       = "count",
    };

    NYA_ArgCommand root = {
      .is_root    = true,
      .parameters = { &count },
    };

    NYA_ArgParser parser = {
      .name         = "test_app",
      .root_command = &root,
    };

    const char*     argv[] = { "app", "--count", "42" };
    NYA_ArgCommand* cmd    = nya_args_parse_argv(&parser, 3, (NYA_CString*)argv);
    nya_assert(cmd != nullptr);
    nya_assert(count.value.as_integer == 42);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: float flag parameter
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_ArgParameter rate = {
      .kind       = NYA_ARG_PARAMETER_KIND_FLAG,
      .value.type = NYA_TYPE_FLOAT,
      .name       = "rate",
    };

    NYA_ArgCommand root = {
      .is_root    = true,
      .parameters = { &rate },
    };

    NYA_ArgParser parser = {
      .name         = "test_app",
      .root_command = &root,
    };

    const char*     argv[] = { "app", "--rate", "3.14159" };
    NYA_ArgCommand* cmd    = nya_args_parse_argv(&parser, 3, (NYA_CString*)argv);
    nya_assert(cmd != nullptr);
    nya_assert(rate.value.as_float > 3.14 && rate.value.as_float < 3.15);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: string flag parameter
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_ArgParameter output = {
      .kind       = NYA_ARG_PARAMETER_KIND_FLAG,
      .value.type = NYA_TYPE_STRING,
      .name       = "output",
    };

    NYA_ArgCommand root = {
      .is_root    = true,
      .parameters = { &output },
    };

    NYA_ArgParser parser = {
      .name         = "test_app",
      .root_command = &root,
    };

    const char*     argv[] = { "app", "--output", "file.txt" };
    NYA_ArgCommand* cmd    = nya_args_parse_argv(&parser, 3, (NYA_CString*)argv);
    nya_assert(cmd != nullptr);
    nya_assert(strcmp(output.value.as_string, "file.txt") == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: multiple flags
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_ArgParameter flag1 = {
      .kind       = NYA_ARG_PARAMETER_KIND_FLAG,
      .value.type = NYA_TYPE_BOOL,
      .name       = "flag1",
    };
    NYA_ArgParameter flag2 = {
      .kind       = NYA_ARG_PARAMETER_KIND_FLAG,
      .value.type = NYA_TYPE_BOOL,
      .name       = "flag2",
    };
    NYA_ArgParameter flag3 = {
      .kind       = NYA_ARG_PARAMETER_KIND_FLAG,
      .value.type = NYA_TYPE_BOOL,
      .name       = "flag3",
    };

    NYA_ArgCommand root = {
      .is_root    = true,
      .parameters = { &flag1, &flag2, &flag3 },
    };

    NYA_ArgParser parser = {
      .name         = "test_app",
      .root_command = &root,
    };

    const char*     argv[] = { "app", "--flag1", "--flag2", "--flag3" };
    NYA_ArgCommand* cmd    = nya_args_parse_argv(&parser, 4, (NYA_CString*)argv);
    nya_assert(cmd != nullptr);
    nya_assert(flag1.value.as_bool == true);
    nya_assert(flag2.value.as_bool == true);
    nya_assert(flag3.value.as_bool == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: subcommand
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_ArgParameter verbose = {
      .kind       = NYA_ARG_PARAMETER_KIND_FLAG,
      .value.type = NYA_TYPE_BOOL,
      .name       = "verbose",
    };

    NYA_ArgCommand build = {
      .name        = "build",
      .description = "Build project",
      .parameters  = { &verbose },
    };

    NYA_ArgCommand root = {
      .is_root     = true,
      .subcommands = { &build },
    };

    NYA_ArgParser parser = {
      .name         = "test_app",
      .root_command = &root,
    };

    const char*     argv[] = { "app", "build", "--verbose" };
    NYA_ArgCommand* cmd    = nya_args_parse_argv(&parser, 3, (NYA_CString*)argv);
    nya_assert(cmd != nullptr);
    nya_assert(cmd == &build);
    nya_assert(verbose.value.as_bool == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: positional parameter
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_ArgParameter input = {
      .kind       = NYA_ARG_PARAMETER_KIND_POSITIONAL,
      .value.type = NYA_TYPE_STRING,
      .name       = "input",
    };

    NYA_ArgCommand root = {
      .is_root    = true,
      .parameters = { &input },
      .handler    = (void*)1, // dummy handler to satisfy validation
    };

    NYA_ArgParser parser = {
      .name         = "test_app",
      .root_command = &root,
    };

    const char*     argv[] = { "app", "input.txt" };
    NYA_ArgCommand* cmd    = nya_args_parse_argv(&parser, 2, (NYA_CString*)argv);
    nya_assert(cmd != nullptr);
    nya_assert(strcmp(input.value.as_string, "input.txt") == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: variadic positional parameter
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_ArgParameter files = {
      .kind       = NYA_ARG_PARAMETER_KIND_POSITIONAL,
      .variadic   = true,
      .value.type = NYA_TYPE_STRING,
      .name       = "files",
    };

    NYA_ArgCommand root = {
      .is_root    = true,
      .parameters = { &files },
      .handler    = (void*)1, // dummy handler to satisfy validation
    };

    NYA_ArgParser parser = {
      .name         = "test_app",
      .root_command = &root,
    };

    const char*     argv[] = { "app", "file1.txt", "file2.txt", "file3.txt" };
    NYA_ArgCommand* cmd    = nya_args_parse_argv(&parser, 4, (NYA_CString*)argv);
    nya_assert(cmd != nullptr);
    nya_assert(files.values_count == 3);
    nya_assert(strcmp(files.values[0].as_string, "file1.txt") == 0);
    nya_assert(strcmp(files.values[1].as_string, "file2.txt") == 0);
    nya_assert(strcmp(files.values[2].as_string, "file3.txt") == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // CLEANUP
  // ─────────────────────────────────────────────────────────────────────────────
  nya_arena_destroy(arena);

  return 0;
}
