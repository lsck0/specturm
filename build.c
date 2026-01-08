#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * COMMON FLAGS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define PROJECT_NAME "gnyame"
#define PROJECT_PATH "./src/main.c"
#define VERSION      "0.0.0"

#define DEBUG_BINARY          PROJECT_NAME "." VERSION ".debug"
#define WINDOWS_X86_64_BINARY PROJECT_NAME "." VERSION ".windows-x86_64.exe"
#define LINUX_X86_64_BINARY   PROJECT_NAME "." VERSION ".linux-x86_64"

// clang-format off
#define CC            "clang"
#define CFLAGS        "-std=c23", "-ggdb", "-fenable-matrix", "-mavx", "-mavx2"
#define WARNINGS      "-pedantic", "-Wall", "-Wextra", "-Wpedantic", "-Wno-gnu", "-Wno-gcc-compat", "-Wno-initializer-overrides", "-Wno-keyword-macro"
#define INCLUDE_PATHS "-I./src/", "-I./vendor/sdl/include/"
#define LINKER_FLAGS  "-lm", "-pthread", "-lSDL3"
#define NPROCS        "16"

#define FLAGS_DEBUG   "-O0", "-DIS_DEBUG=true", "-fno-omit-frame-pointer", "-fno-optimize-sibling-calls", "-fno-sanitize-recover=all", "-fsanitize=address,leak,undefined,signed-integer-overflow,unsigned-integer-overflow,shift,float-cast-overflow,float-divide-by-zero,pointer-overflow"
#define FLAGS_RELEASE "-O3", "-D_FORTIFY_SOURCE=2", "-fno-omit-frame-pointer", "-fstack-protector-strong"

#define FLAGS_WINDOWS_X86_64 "--target=x86_64-w64-mingw32", "-Wl,-subsystem,windows", "-static", "-L./vendor/sdl/build-window-x86_64/", "-lcomdlg32", "-ldxguid", "-lgdi32", "-limm32", "-lkernel32", "-lole32", "-loleaut32", "-lsetupapi", "-luser32", "-luuid", "-lversion", "-lwinmm"
#define FLAGS_LINUX_X86_64   "-L./vendor/sdl/build-linux-x86_64/"
// clang-format on

NYA_INTERNAL void add_version_flag_and_git_hash(NYA_BuildRule* rule);
NYA_INTERNAL void convert_perf_data_to_plain(NYA_BuildRule* rule);
NYA_INTERNAL void remove_output_file(NYA_BuildRule* rule);
NYA_INTERNAL void test_runner(NYA_BuildRule* rule);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * BUILD RULES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */
// for some reason clang-format shits itself formatting the build rules
// clang-format off

NYA_Command build_rebuild_command = {
    .program   = CC,
    .arguments = {
        "build.c",
        "-o", "build",
        CFLAGS,
        WARNINGS,
        INCLUDE_PATHS,
        LINKER_FLAGS,
        FLAGS_DEBUG,
    },
};

/*
 * ─────────────────────────────────────────────────────────
 * VENDOR RULES
 * ─────────────────────────────────────────────────────────
 */

NYA_BuildRule build_prepare_linux_x86_64_sdl = {
    .name        = "build_prepare_linux_x86_64_sdl",
    .policy      = NYA_BUILD_ONCE,
    .output_file = "./vendor/sdl/build-linux-x86_64/libSDL3.a",

    .command = {
        .program   = "cmake",
        .arguments = {
            "-S", "./vendor/sdl",
            "-B", "./vendor/sdl/build-linux-x86_64/",
            "-GNinja",
            "-DCMAKE_BUILD_TYPE=Release",
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON",
            "-DSDL_SHARED=OFF",
            "-DSDL_STATIC=ON",
        },
    },
};

NYA_BuildRule build_linux_x64_64_sdl = {
    .name        = "build_linux_x64_64_sdl",
    .policy      = NYA_BUILD_ONCE,
    .output_file = "./vendor/sdl/build-linux-x86_64/libSDL3.a",

    .command = {
        .program   = "cmake",
        .arguments = {
            "--build", "./vendor/sdl/build-linux-x86_64/",
            "--config", "Release",
            "--", "-j", NPROCS,
        },
    },

    .dependencies = { &build_prepare_linux_x86_64_sdl },
};

NYA_BuildRule build_prepare_windows_x86_64_sdl = {
    .name        = "build_prepare_windows_x86_64_sdl",
    .policy      = NYA_BUILD_ONCE,
    .output_file = "./vendor/sdl/build-window-x86_64/libSDL3.a",

    .command = {
        .program   = "cmake",
        .arguments = {
            "-S", "./vendor/sdl",
            "-B", "./vendor/sdl/build-window-x86_64/",
            "-GNinja",
            "-DCMAKE_BUILD_TYPE=Release",
            "-DCMAKE_POSITION_INDEPENDENT_CODE=ON",
            "-DSDL_SHARED=OFF",
            "-DSDL_STATIC=ON",
            "-DCMAKE_SYSTEM_NAME=Windows",
            "-DCMAKE_C_COMPILER=/usr/bin/x86_64-w64-mingw32-gcc",
            "-DCMAKE_CXX_COMPILER=/usr/bin/x86_64-w64-mingw32-g++",
            "-DCMAKE_LINKER=/usr/bin/x86_64-w64-mingw32-ld",
            "-DCMAKE_RC_COMPILER=/usr/bin/x86_64-w64-mingw32-windres",
            "-DCMAKE_FIND_ROOT_PATH=/usr/x86_64-w64-mingw32",
            "-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=BOTH",
            "-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY",
            "-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=BOTH",
        },
    },
};

NYA_BuildRule build_windows_x86_64_sdl = {
    .name        = "build_windows_x86_64_sdl",
    .policy      = NYA_BUILD_ONCE,
    .output_file = "./vendor/sdl/build-window-x86_64/libSDL3.a",

    .command = {
        .program   = "cmake",
        .arguments = {
            "--build", "./vendor/sdl/build-window-x86_64/",
            "--config", "Release",
            "--", "-j", NPROCS,
        },
    },

    .dependencies = { &build_prepare_windows_x86_64_sdl },
};

NYA_BuildRule build_prepare_shadercross_linux_x86_64 = {
    .name        = "build_prepare_shadercross_linux_x86_64",
    .policy      = NYA_BUILD_ONCE,
    .output_file = "./vendor/sdl-shadercross/build/shadercross",

    .command = {
        .program   = "cmake",
        .arguments = {
            "-S", "./vendor/sdl-shadercross",
            "-B", "./vendor/sdl-shadercross/build",
            "-GNinja",
            "-DSDLSHADERCROSS_VENDORED=ON",
        },
    },
};

NYA_BuildRule build_shadercross_linux_x86_64 = {
    .name        = "build_shadercross_linux_x86_64",
    .policy      = NYA_BUILD_ONCE,
    .output_file = "./vendor/sdl-shadercross/build/shadercross",

    .command = {
        .program   = "cmake",
        .arguments = {
            "--build", "./vendor/sdl-shadercross/build",
            "--config", "Release",
            "--", "-j", NPROCS,
        },
    },

    .dependencies = { &build_prepare_shadercross_linux_x86_64 },
};

/*
 * ─────────────────────────────────────────────────────────
 * ASSET RULES
 * ─────────────────────────────────────────────────────────
 */

NYA_BuildRule build_windows_icon = {
    .name        = "build_windows_icon",
    .policy      = NYA_BUILD_IF_OUTDATED,
    .input_file  = "./assets/icons/icon.ico",
    .output_file = "./assets/icons/icon.res",

    .command = {
        .program   = "x86_64-w64-mingw32-windres",
        .arguments = {
            "./assets/icons/icon.rc",
            "-O", "coff",
            "-o", "./assets/icons/icon.res",
        },
    },
};

/*
 * ─────────────────────────────────────────────────────────
 * PROJECT BUILD RULES
 * ─────────────────────────────────────────────────────────
 */

NYA_BuildRule build_docs = {
    .name    = "build_docs",
    .policy  = NYA_BUILD_ALWAYS,

    .command = {
        .program   = "doxygen",
        .arguments = { "./docs/doxygen.config" },
    },
};

NYA_BuildRule build_project_debug = {
    .name   = "build_project_debug",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = CC,
        .arguments = {
            PROJECT_PATH,
            "-o", DEBUG_BINARY,
            CFLAGS,
            WARNINGS,
            INCLUDE_PATHS,
            LINKER_FLAGS,
            FLAGS_DEBUG,
            FLAGS_LINUX_X86_64,
        },
    },

    .pre_build_hooks = { &add_version_flag_and_git_hash },
    .dependencies    = { &build_linux_x64_64_sdl },
};

NYA_BuildRule build_project_linux_x86_64 = {
    .name   = "build_project_linux_x86_64",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = CC,
        .arguments = {
            PROJECT_PATH,
            "-o", LINUX_X86_64_BINARY,
            CFLAGS,
            WARNINGS,
            INCLUDE_PATHS,
            LINKER_FLAGS,
            FLAGS_RELEASE,
            FLAGS_LINUX_X86_64,
        },
    },

    .pre_build_hooks = { &add_version_flag_and_git_hash },
    .dependencies    = { &build_linux_x64_64_sdl },
};

NYA_BuildRule build_project_windows_x86_64 = {
    .name   = "build_project_windows_x86_64",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = CC,
        .arguments = {
            PROJECT_PATH,
            "-o", WINDOWS_X86_64_BINARY,
            CFLAGS,
            WARNINGS,
            INCLUDE_PATHS,
            LINKER_FLAGS,
            FLAGS_RELEASE,
            FLAGS_WINDOWS_X86_64,
            "./assets/icons/icon.res",
        },
    },

    .pre_build_hooks = { &add_version_flag_and_git_hash },
    .dependencies    = { &build_windows_x86_64_sdl, &build_windows_icon },
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TARGETS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_BuildRule run_debug = {
    .name   = "run_debug",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program = "perf",
        .arguments = {
            "record",
            "-T",
            "-F", "100",
            "-g", "--call-graph", "dwarf",
            "-e", "cycles,instructions,cache-misses",
            "./" DEBUG_BINARY,
        },
        .environment = {
            "ASAN_OPTIONS=suppressions=./.sanitizers/asan.supp:detect_leaks=1:strict_string_checks=1:halt_on_error=1",
            "LSAN_OPTIONS=suppressions=./.sanitizers/lsan.supp",
            "TSAN_OPTIONS=suppressions=./.sanitizers/tsan.supp",
            "UBSAN_OPTIONS=suppressions=./.sanitizers/ubsan.supp:print_stacktrace=1:halt_on_error=1",
        },
    },

    .dependencies = { &build_project_debug },

    .post_build_hooks = { &convert_perf_data_to_plain },
};

NYA_BuildRule run_tests = {
    .name   = "run_tests",
    .is_metarule  = true,
    .pre_build_hooks = { &test_runner },
};

NYA_BuildRule build_project = {
    .name         = "build_project",
    .is_metarule  = true,
    .dependencies = {
        &build_project_linux_x86_64,
        &build_project_windows_x86_64,
    },
};

NYA_BuildRule open_perf_report = {
    .name        = "open_perf_report",
    .is_metarule = true,
    .dependencies = {
        &(NYA_BuildRule){
            .name   = "open_speedscope",
            .policy = NYA_BUILD_ALWAYS,

            .command = {
                .program   = "speedscope",
                .arguments = {"./perf.data.txt"},
            },
        },
        &(NYA_BuildRule){
            .name   = "open_hotspot",
            .policy = NYA_BUILD_ALWAYS,

            .command = {
                .program   = "hotspot",
                .arguments = {"./perf.data"},
            },
        },
    },
};

NYA_BuildRule open_docs = {
    .name   = "open_docs",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = "xdg-open",
        .arguments = { "./docs/doxygen/html/index.html" },
    },

    .dependencies = { &build_docs },
};

NYA_BuildRule show_stats = {
    .name   = "show_stats",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = "tokei",
        .arguments = { ".", "--exclude", "vendor" },
    },
};

// clang-format on
/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * HOOKS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL void add_version_flag_and_git_hash(NYA_BuildRule* rule) {
  nya_assert(rule);

  static b8          initialized = false;
  static NYA_CString GIT_HASH_FLAG;
  static NYA_CString VERSION_FLAG;
  if (initialized) goto skip_initialization;

  NYA_Command git_hash_command = {
      .arena     = &nya_global_arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "git",
      .arguments = {"rev-parse", "HEAD"},
  };
  nya_command_run(&git_hash_command);
  nya_string_trim_whitespace(&git_hash_command.stdout_content);
  NYA_CString git_hash      = nya_string_to_cstring(&nya_global_arena, &git_hash_command.stdout_content);
  NYA_String  git_hash_flag = nya_string_sprintf(&nya_global_arena, "-DGIT_COMMIT=\"%s\"", git_hash);
  NYA_String  version_flag  = nya_string_sprintf(&nya_global_arena, "-DVERSION=\"%s\"", VERSION);
  GIT_HASH_FLAG             = nya_string_to_cstring(&nya_global_arena, &git_hash_flag);
  VERSION_FLAG              = nya_string_to_cstring(&nya_global_arena, &version_flag);
  initialized               = true;

skip_initialization:

  u64 length = 0;
  while (length < NYA_COMMAND_MAX_ARGUMENTS && rule->command.arguments[length] != nullptr) length++;
  nya_assert(length < NYA_COMMAND_MAX_ARGUMENTS - 2, "Not enough space to add version flags.");
  rule->command.arguments[length + 0] = GIT_HASH_FLAG;
  rule->command.arguments[length + 1] = VERSION_FLAG;
}

NYA_INTERNAL void convert_perf_data_to_plain(NYA_BuildRule* rule) {
  nya_assert(rule);

  NYA_Command command = {
      .arena     = &nya_global_arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "perf",
      .arguments = {"script", "-i", "./perf.data"},
  };
  nya_command_run(&command);

  nya_file_write("./perf.data.txt", &command.stdout_content);
}

NYA_INTERNAL void remove_output_file(NYA_BuildRule* rule) {
  nya_assert(rule);
  nya_assert(rule->output_file);

  nya_filesystem_file_delete(rule->output_file);
}

NYA_INTERNAL void test_runner(NYA_BuildRule* rule) {
  nya_assert(rule);

  NYA_Command find_tests_command = {
      .arena     = &nya_global_arena,
      .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
      .program   = "find",
      .arguments = {"./tests/", "-name", "*.c"},
  };
  nya_command_run(&find_tests_command);
  NYA_StringArray tests = nya_string_split_lines(&nya_global_arena, &find_tests_command.stdout_content);

  nya_array_foreach (&tests, test) {
    NYA_CString test_cstr = nya_string_to_cstring(&nya_global_arena, test);

    nya_string_strip_suffix(test, ".c");
    NYA_CString test_binary = nya_string_to_cstring(&nya_global_arena, test);

    // clang-format off
    NYA_String    build_test_name = nya_string_sprintf(&nya_global_arena, "build_test:%s", test_binary);
    NYA_BuildRule build_test = {
        .name        = nya_string_to_cstring(&nya_global_arena, &build_test_name),
        .policy      = NYA_BUILD_ALWAYS,
        .output_file = test_binary,

        .command = {
            .program   = CC,
            .arguments = {
                test_cstr,
                "-o", test_binary,
                CFLAGS,
                WARNINGS,
                INCLUDE_PATHS,
                LINKER_FLAGS,
                FLAGS_DEBUG,
                FLAGS_LINUX_X86_64,
            },
        },

        .pre_build_hooks = { &add_version_flag_and_git_hash },
    };

    NYA_String    run_test_name = nya_string_sprintf(&nya_global_arena, "run_test:%s", test_binary);
    NYA_BuildRule run_test = {
        .name        = nya_string_to_cstring(&nya_global_arena, &run_test_name),
        .policy      = NYA_BUILD_ALWAYS,
        .output_file = test_binary,

        .command = {
            .program     = test_binary,
            .environment = {
              "ASAN_OPTIONS=suppressions=./.sanitizers/asan.supp:detect_leaks=1:strict_string_checks=1:halt_on_error=1",
              "LSAN_OPTIONS=suppressions=./.sanitizers/lsan.supp",
              "TSAN_OPTIONS=suppressions=./.sanitizers/tsan.supp",
              "UBSAN_OPTIONS=suppressions=./.sanitizers/ubsan.supp:print_stacktrace=1:halt_on_error=1",
            },
        },

        .dependencies      = { &build_test },
        .post_build_hooks  = { &remove_output_file },
    };
    // clang-format on

    nya_build(&run_test);
  }
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ENTRY POINT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL void usage_and_exit(NYA_CString program_name) {
  printf("Usage: %s [task]\n", program_name);
  printf("Tasks:\n");
  printf("  run:debug\n");
  printf("  run:tests\n");
  printf("  open:perf\n");
  printf("  open:docs\n");
  printf("  open:stats\n");
  printf("  build:debug\n");
  printf("  build:release\n");

  exit(EXIT_FAILURE);
}

s32 main(s32 argc, NYA_CString* argv) {
  // TODO: add a way to skip this for faster rebuilds during development
  nya_rebuild_yourself(&argc, argv, build_rebuild_command);

  if (argc != 2) usage_and_exit(argv[0]);

  NYA_CString task = argv[1];
  if (nya_string_equals(task, "run:debug")) {
    nya_build(&run_debug);
  } else if (nya_string_equals(task, "run:tests")) {
    nya_build(&run_tests);
  } else if (nya_string_equals(task, "open:perf")) {
    nya_build(&open_perf_report);
  } else if (nya_string_equals(task, "open:docs")) {
    nya_build(&open_docs);
  } else if (nya_string_equals(task, "open:stats")) {
    nya_build(&show_stats);
  } else if (nya_string_equals(task, "build:debug")) {
    nya_build(&build_project_debug);
  } else if (nya_string_equals(task, "build:release")) {
    nya_build(&build_project);
  } else {
    usage_and_exit(argv[0]);
  }

  return EXIT_SUCCESS;
}
