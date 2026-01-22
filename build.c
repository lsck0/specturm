#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * COMMON FLAGS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define PROJECT_NAME "gnyame"
#define VERSION      "0.0.0"

#define BINARY_SOURCE_PATH "./src/main.c"
#define DLL_SOURCE_PATH    "./src/gnyame/gnyame.c"

#define DEBUG_BINARY          PROJECT_NAME ".debug"
#define DEBUG_DLL             PROJECT_NAME ".debug.so"
#define WINDOWS_X86_64_BINARY PROJECT_NAME "." VERSION ".windows-x86_64.exe"
#define LINUX_X86_64_BINARY   PROJECT_NAME "." VERSION ".linux-x86_64"

#define CC            "clang"
#define CFLAGS        "-std=c23", "-ggdb", "-fenable-matrix", "-mavx", "-mavx2"
#define WARNINGS      "-pedantic", "-Wall", "-Wextra", "-Wpedantic", "-Wno-gnu", "-Wno-gcc-compat", "-Wno-initializer-overrides", "-Wno-keyword-macro"
#define INCLUDE_PATHS "-I./src/", "-I./", "-I./vendor/sdl/include/"
#define LINKER_FLAGS  "-lm", "-pthread", "-lSDL3"
#define NPROCS        "16"

// clang-format off
#define FLAGS_DEBUG     "-O0", "-DIS_DEBUG=true", "-rdynamic"
#define FLAGS_DLL       "-fPIC", "-shared"
#define FLAGS_SANITIZE  "-fno-omit-frame-pointer", "-fno-optimize-sibling-calls", "-fno-sanitize-recover=all", "-fsanitize=address,leak,undefined,signed-integer-overflow,unsigned-integer-overflow,shift,float-cast-overflow,float-divide-by-zero,pointer-overflow"
#define FLAGS_RELEASE   "-O3", "-D_FORTIFY_SOURCE=2", "-fno-omit-frame-pointer", "-fstack-protector-strong", "-Wl,-rpath,$ORIGIN"
#define FLAGS_WINDOWS_X86_64 "--target=x86_64-w64-mingw32", "-Wl,-subsystem,windows", "-static", "-L./vendor/sdl/build-window-x86_64/", "-lcomdlg32", "-ldxguid", "-lgdi32", "-limm32", "-lkernel32", "-lole32", "-loleaut32", "-lsetupapi", "-luser32", "-luuid", "-lversion", "-lwinmm"
#define FLAGS_LINUX_X86_64   "-L./vendor/sdl/build-linux-x86_64/"
// clang-format on

NYA_INTERNAL void hook_compile_shaders(NYA_BuildRule* rule);
NYA_INTERNAL void hook_index_assets(NYA_BuildRule* rule);
NYA_INTERNAL void hook_bundle_assets(NYA_BuildRule* rule);
NYA_INTERNAL void hook_add_version_flag_and_git_hash(NYA_BuildRule* rule);
NYA_INTERNAL void hook_convert_perf_data_to_plain(NYA_BuildRule* rule);
NYA_INTERNAL void hook_remove_output_file(NYA_BuildRule* rule);
NYA_INTERNAL void hook_bundle_project(NYA_BuildRule* rule);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * BUILD RULES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_Command build_rebuild_command = {
    .program   = CC,
    .arguments = {
        "build.c",
        "-o", "build",
        "-DIS_DEBUG=true",
        CFLAGS,
        WARNINGS,
        INCLUDE_PATHS,
        LINKER_FLAGS,
        FLAGS_LINUX_X86_64,
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

    .dependencies = { &build_prepare_linux_x86_64_sdl, },
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

    .dependencies = { &build_prepare_windows_x86_64_sdl, },
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

    .dependencies = { &build_prepare_shadercross_linux_x86_64, },
};

/*
 * ─────────────────────────────────────────────────────────
 * ASSET RULES
 * ─────────────────────────────────────────────────────────
 */

NYA_BuildRule build_windows_icon = {
    .name        = "build_windows_icon",
    .policy      = NYA_BUILD_IF_OUTDATED,
    .input_file  = "./assets/icon/icon.ico",
    .output_file = "./assets/icon/icon.res",

    .command = {
        .program   = "x86_64-w64-mingw32-windres",
        .arguments = {
            "./assets/icon/icon.rc",
            "-O", "coff",
            "-o", "./assets/icon/icon.res",
        },
    },
};

NYA_BuildRule build_shaders = {
    .name            = "build_shaders",
    .is_metarule     = true,
    .dependencies    = { &build_shadercross_linux_x86_64, },
    .pre_build_hooks = { &hook_compile_shaders, },
};

NYA_BuildRule index_assets = {
    .name             = "index_assets",
    .is_metarule      = true,
    .dependencies     = { &build_windows_icon, &build_shaders, },
    .post_build_hooks = { &hook_index_assets, },
};

NYA_BuildRule bundle_assets = {
    .name             = "bundle_assets",
    .is_metarule      = true,
    .dependencies     = { &build_windows_icon, &build_shaders, },
    .post_build_hooks = { &hook_bundle_assets, },
};

/*
 * ─────────────────────────────────────────────────────────
 * PROJECT BUILD RULES
 * ─────────────────────────────────────────────────────────
 */

NYA_BuildRule build_project_debug_executable = {
    .name   = "build_project_debug_executable",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = CC,
        .arguments = {
            BINARY_SOURCE_PATH,
            "-o", DEBUG_BINARY,
            CFLAGS,
            WARNINGS,
            INCLUDE_PATHS,
            LINKER_FLAGS,
            FLAGS_SANITIZE,
            FLAGS_DEBUG,
            FLAGS_LINUX_X86_64,
        },
    },

    .pre_build_hooks = { &hook_add_version_flag_and_git_hash, },
    .dependencies    = { &build_linux_x64_64_sdl, },
};

NYA_BuildRule build_project_debug_dll = {
    .name   = "build_project_debug_dll",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = CC,
        .arguments = {
            DLL_SOURCE_PATH,
            "-o", DEBUG_DLL,
            CFLAGS,
            WARNINGS,
            INCLUDE_PATHS,
            LINKER_FLAGS,
            FLAGS_SANITIZE,
            FLAGS_DEBUG,
            FLAGS_DLL,
            FLAGS_LINUX_X86_64,
        },
    },

    .pre_build_hooks = { &hook_add_version_flag_and_git_hash, },
    .dependencies    = { &build_linux_x64_64_sdl, &build_shaders, &index_assets, },
};

NYA_BuildRule build_project_debug = {
    .name   = "build_project_debug",
    .is_metarule  = true,
    .dependencies    = { &build_project_debug_executable, &build_project_debug_dll, },
};

NYA_BuildRule build_project_linux_x86_64 = {
    .name   = "build_project_linux_x86_64",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = CC,
        .arguments = {
            BINARY_SOURCE_PATH,
            "-o", LINUX_X86_64_BINARY,
            CFLAGS,
            WARNINGS,
            INCLUDE_PATHS,
            LINKER_FLAGS,
            FLAGS_RELEASE,
            FLAGS_LINUX_X86_64,
        },
    },

    .pre_build_hooks = { &hook_add_version_flag_and_git_hash, },
    .dependencies    = { &build_linux_x64_64_sdl, &bundle_assets, &index_assets, },
};

NYA_BuildRule build_project_windows_x86_64 = {
    .name   = "build_project_windows_x86_64",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = CC,
        .arguments = {
            BINARY_SOURCE_PATH,
            "-o", WINDOWS_X86_64_BINARY,
            CFLAGS,
            WARNINGS,
            INCLUDE_PATHS,
            LINKER_FLAGS,
            FLAGS_RELEASE,
            FLAGS_WINDOWS_X86_64,
            "./assets/icon/icon.res",
        },
    },

    .pre_build_hooks = { &hook_add_version_flag_and_git_hash, },
    .dependencies    = { &build_windows_x86_64_sdl, &bundle_assets, &index_assets, },
};

NYA_BuildRule build_project = {
    .name         = "build_project",
    .is_metarule  = true,
    .dependencies = {
        &build_project_linux_x86_64,
        &build_project_windows_x86_64,
    },
};

NYA_BuildRule bundle_project = {
    .name         = "bundle_project",
    .is_metarule  = true,
    .dependencies = { &build_project, },
    .post_build_hooks = { &hook_bundle_project, },
};

NYA_BuildRule build_docs = {
    .name    = "build_docs",
    .policy  = NYA_BUILD_ALWAYS,

    .command = {
        .program   = "doxygen",
        .arguments = { "./docs/doxygen.config", },
    },
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * OTHER TARGETS
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

    .dependencies = { &build_project_debug, },

    .post_build_hooks = { &hook_convert_perf_data_to_plain, },
};

NYA_BuildRule run_release = {
    .name   = "run_release",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program = "./" LINUX_X86_64_BINARY,
    },

    .dependencies = { &build_project_linux_x86_64, },
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
                .arguments = { "./perf.data.txt", },
            },
        },
        &(NYA_BuildRule){
            .name   = "open_hotspot",
            .policy = NYA_BUILD_ALWAYS,

            .command = {
                .program   = "hotspot",
                .arguments = { "./perf.data", },
            },
        },
    },
};

NYA_BuildRule open_docs = {
    .name   = "open_docs",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = "xdg-open",
        .arguments = { "./docs/doxygen/html/index.html", },
    },

    .dependencies = { &build_docs, },
};

NYA_BuildRule show_stats = {
    .name   = "show_stats",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = "tokei",
        .arguments = { ".", "--exclude", "vendor", },
    },
};

NYA_BuildRule update_submodules = {
    .name   = "update_submodules",
    .policy = NYA_BUILD_ALWAYS,

    .command = {
        .program   = "git",
        .arguments = { "submodule", "foreach", "git", "pull", "origin", "main", },
    },
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * HOOKS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL void hook_compile_shaders(NYA_BuildRule* rule) {
  nya_assert(rule);

  NYA_Command find_source_shaders = {
    .arena     = &nya_arena_global,
    .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
    .program   = "find",
    .arguments = { "./assets/shaders/source/", "-name", "*.hlsl", },
  };
  nya_command_run(&find_source_shaders);
  nya_assert(find_source_shaders.exit_code == 0, "Failed to find source shaders.");
  NYA_StringArray shaders = nya_string_split_lines(&nya_arena_global, &find_source_shaders.stdout_content);

  nya_array_foreach (&shaders, shader) {
    NYA_CString source = nya_string_to_cstring(&nya_arena_global, shader);
    nya_string_strip_prefix(shader, "./assets/shaders/source/");
    nya_string_strip_suffix(shader, ".hlsl");

    nya_string_extend_front(shader, "./assets/shaders/compiled/DXIL/");
    nya_string_extend(shader, ".dxil");
    NYA_CString target_dxil = nya_string_to_cstring(&nya_arena_global, shader);
    nya_string_strip_prefix(shader, "./assets/shaders/compiled/DXIL/");
    nya_string_strip_suffix(shader, ".dxil");

    nya_string_extend_front(shader, "./assets/shaders/compiled/MSL/");
    nya_string_extend(shader, ".msl");
    NYA_CString target_metal = nya_string_to_cstring(&nya_arena_global, shader);
    nya_string_strip_suffix(shader, ".msl");
    nya_string_strip_prefix(shader, "./assets/shaders/compiled/MSL/");

    nya_string_extend_front(shader, "./assets/shaders/compiled/SPIRV/");
    nya_string_extend(shader, ".spv");
    NYA_CString target_spirv = nya_string_to_cstring(&nya_arena_global, shader);
    nya_string_strip_suffix(shader, ".spv");
    nya_string_strip_prefix(shader, "./assets/shaders/compiled/SPIRV/");

    NYA_Command create_dirs = {
        .program   = "mkdir",
        .arguments = {
            "-p",
            "./assets/shaders/compiled/DXIL/",
            "./assets/shaders/compiled/MSL/",
            "./assets/shaders/compiled/SPIRV/",
        },
    };
    nya_command_run(&create_dirs);
    nya_assert(create_dirs.exit_code == 0, "Failed to create shader output directories.");

    // compile to DXIL
    NYA_String compile_to_dxil_name = nya_string_sprintf(&nya_arena_global, "%s -> %s", source, target_dxil);
    NYA_BuildRule compile_to_dxil      = {
        .name        = nya_string_to_cstring(&nya_arena_global, &compile_to_dxil_name),
        .policy      = NYA_BUILD_IF_OUTDATED,
        .input_file  = source,
        .output_file = target_dxil,
        .command = {
            .program = "./vendor/sdl-shadercross/build/shadercross",
            .arguments = {
                source,
                "-o", target_dxil,
                "-s", "hlsl",
                "-d", "dxil",
            },
        },
    };
    nya_build(&compile_to_dxil);

    // compile to Metal
    NYA_String compile_to_metal_name = nya_string_sprintf(&nya_arena_global, "%s -> %s", source, target_metal);
    NYA_BuildRule compile_to_metal      = {
        .name        = nya_string_to_cstring(&nya_arena_global, &compile_to_metal_name),
        .policy      = NYA_BUILD_IF_OUTDATED,
        .input_file  = source,
        .output_file = target_metal,
        .command = {
            .program = "./vendor/sdl-shadercross/build/shadercross",
            .arguments = {
                source,
                "-o", target_metal,
                "-s", "hlsl",
                "-d", "msl",
            },
        },
    };

    nya_build(&compile_to_metal);

    // compile to SPIR-V
    NYA_String compile_to_spirv_name = nya_string_sprintf(&nya_arena_global, "%s -> %s", source, target_spirv);
    NYA_BuildRule compile_to_spirv      = {
        .name        = nya_string_to_cstring(&nya_arena_global, &compile_to_spirv_name),
        .policy      = NYA_BUILD_IF_OUTDATED,
        .input_file  = source,
        .output_file = target_spirv,
        .command = {
            .program = "./vendor/sdl-shadercross/build/shadercross",
            .arguments = {
                source,
                "-o", target_spirv,
                "-s", "hlsl",
                "-d", "spirv",
            },
        },
    };
    nya_build(&compile_to_spirv);
  }
}

NYA_INTERNAL void hook_index_assets(NYA_BuildRule* rule) {
  nya_assert(rule);

  NYA_ConstCString asset_directory = "./assets/";
  NYA_ConstCString output_file     = "./assets/assets.h";

  nya_asset_generate_manifest(asset_directory, output_file);
}

NYA_INTERNAL void hook_bundle_assets(NYA_BuildRule* rule) {
  nya_assert(rule);

  NYA_ConstCString asset_directory = "./assets/";
  NYA_ConstCString output_file     = "./assets/assets.c";

  nya_asset_generate_embedding(asset_directory, output_file);
}

NYA_INTERNAL void hook_add_version_flag_and_git_hash(NYA_BuildRule* rule) {
  nya_assert(rule);

  static b8          initialized = false;
  static NYA_CString GIT_HASH_FLAG;
  static NYA_CString VERSION_FLAG;
  if (initialized) goto skip_initialization;

  NYA_Command git_hash_command = {
    .arena     = &nya_arena_global,
    .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
    .program   = "git",
    .arguments = { "rev-parse", "HEAD" },
  };
  nya_command_run(&git_hash_command);
  nya_assert(git_hash_command.exit_code == 0, "Failed to get git commit hash.");

  nya_string_trim_whitespace(&git_hash_command.stdout_content);
  NYA_CString git_hash      = nya_string_to_cstring(&nya_arena_global, &git_hash_command.stdout_content);
  NYA_String  git_hash_flag = nya_string_sprintf(&nya_arena_global, "-DGIT_COMMIT=\"%s\"", git_hash);
  NYA_String  version_flag  = nya_string_sprintf(&nya_arena_global, "-DVERSION=\"%s\"", VERSION);
  GIT_HASH_FLAG             = nya_string_to_cstring(&nya_arena_global, &git_hash_flag);
  VERSION_FLAG              = nya_string_to_cstring(&nya_arena_global, &version_flag);
  initialized               = true;

skip_initialization:

  u64 length = 0;
  while (length < NYA_COMMAND_MAX_ARGUMENTS && rule->command.arguments[length] != nullptr) length++;
  nya_assert(length < NYA_COMMAND_MAX_ARGUMENTS - 2, "Not enough space to add version flags.");
  rule->command.arguments[length + 0] = GIT_HASH_FLAG;
  rule->command.arguments[length + 1] = VERSION_FLAG;
}

NYA_INTERNAL void hook_convert_perf_data_to_plain(NYA_BuildRule* rule) {
  nya_assert(rule);

  NYA_Command command = {
    .arena     = &nya_arena_global,
    .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
    .program   = "perf",
    .arguments = { "script", "-i", "./perf.data" },
  };
  nya_command_run(&command);
  nya_assert(command.exit_code == 0, "Failed to convert perf data to plain text.");

  nya_file_write("./perf.data.txt", &command.stdout_content);
}

NYA_INTERNAL void hook_remove_output_file(NYA_BuildRule* rule) {
  nya_assert(rule);
  nya_assert(rule->output_file);

  nya_filesystem_delete(rule->output_file);
}

// we just need to move everything into the right place
NYA_INTERNAL void hook_bundle_project(NYA_BuildRule* rule) {
  nya_assert(rule);

  NYA_ConstCString dist_path    = "./dist/";
  NYA_ConstCString linux_path   = "./dist/" PROJECT_NAME "." VERSION ".linux-x86_64/";
  NYA_ConstCString windows_path = "./dist/" PROJECT_NAME "." VERSION ".windows-x86_64/";

  NYA_Command clean_dist = {
    .program   = "rm",
    .arguments = { "-rf", dist_path },
  };
  nya_command_run(&clean_dist);
  nya_assert(clean_dist.exit_code == 0, "Failed to clean dist directory.");

  NYA_Command create_dirs = {
    .program   = "mkdir",
    .arguments = { "-p", linux_path, windows_path, },
  };
  nya_command_run(&create_dirs);
  nya_assert(create_dirs.exit_code == 0, "Failed to create dist directories.");

  // LINUX

  NYA_Command copy_linux_binary = {
    .program   = "cp",
    .arguments = { LINUX_X86_64_BINARY, linux_path, },
  };
  nya_command_run(&copy_linux_binary);
  nya_assert(copy_linux_binary.exit_code == 0, "Failed to copy linux binary.");

  NYA_Command copy_steam_sdk_linux = {
    .program   = "cp",
    .arguments = { "./vendor/steam/redistributable_bin/linux64/libsteam_api.so", linux_path, },
  };
  nya_command_run(&copy_steam_sdk_linux);
  nya_assert(copy_steam_sdk_linux.exit_code == 0, "Failed to copy steam sdk for linux.");

  // WINDOWS

  NYA_Command copy_windows_binary = {
    .program   = "cp",
    .arguments = { WINDOWS_X86_64_BINARY, windows_path },
  };
  nya_command_run(&copy_windows_binary);
  nya_assert(copy_windows_binary.exit_code == 0, "Failed to copy windows binary.");

  NYA_Command copy_steam_sdk_windows = {
    .program   = "cp",
    .arguments = { "./vendor/steam/redistributable_bin/win64/steam_api64.dll", windows_path, },
  };
  nya_command_run(&copy_steam_sdk_windows);
  nya_assert(copy_steam_sdk_windows.exit_code == 0, "Failed to copy steam sdk for windows.");
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TEST RUNNER
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_INTERNAL void test_runner(NYA_ArgCommand* command) {
  nya_assert(command);

  NYA_ArgParameter* test_files = command->parameters[0];
  nya_assert(test_files);
  nya_assert(strcmp(test_files->name, "tests") == 0);

  NYA_Command find_tests_command = {
    .arena     = &nya_arena_global,
    .flags     = NYA_COMMAND_FLAG_OUTPUT_CAPTURE,
    .program   = "find",
    .arguments = { "./tests/", "-name", "*.c" },
  };
  nya_command_run(&find_tests_command);
  NYA_StringArray tests = nya_string_split_lines(&nya_arena_global, &find_tests_command.stdout_content);

  nya_array_foreach (&tests, test) {
    NYA_CString test_cstr = nya_string_to_cstring(&nya_arena_global, test);

    // check if we should run this test, by checking if its name contains any of the requested test names
    b8 should_run = test_files->values_count == 0 ? true : false;
    for (u32 param_index = 0; param_index < test_files->values_count; param_index++) {
      NYA_CString requested_test = test_files->values[param_index].as_string;
      if (nya_string_contains(test, requested_test)) {
        should_run = true;
        break;
      }
    }
    if (!should_run) continue;

    nya_string_strip_suffix(test, ".c");
    NYA_CString test_binary = nya_string_to_cstring(&nya_arena_global, test);

    NYA_String build_test_name = nya_string_sprintf(&nya_arena_global, "build_test:%s", test_binary);
    NYA_BuildRule build_test = {
        .name        = nya_string_to_cstring(&nya_arena_global, &build_test_name),
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
                FLAGS_SANITIZE,
                FLAGS_DEBUG,
                FLAGS_LINUX_X86_64,
                "-Wl,-rpath,$ORIGIN/../../../vendor/steam/redistributable_bin/linux64",
            },
        },

        .pre_build_hooks = { &hook_add_version_flag_and_git_hash, },
    };

    NYA_String run_test_name = nya_string_sprintf(&nya_arena_global, "run_test:%s", test_binary);
    NYA_BuildRule run_test = {
        .name        = nya_string_to_cstring(&nya_arena_global, &run_test_name),
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

        .dependencies      = { &build_test, },
        .post_build_hooks  = { &hook_remove_output_file, },
    };

    b8 ok = nya_build(&run_test);
    if (!ok) nya_panic("TEST FAILED: %s", test_binary);
  }
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * CLI PARSER
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_ArgParameter test_files = {
  .kind        = NYA_ARG_PARAMETER_KIND_POSITIONAL,
  .variadic    = true,
  .value.type  = NYA_TYPE_STRING,
  .name        = "tests",
  .description = "Which tests to run. If none specified, all tests are run.",
};

NYA_ArgParameter skip_self_rebuild_flag = {
  .kind        = NYA_ARG_PARAMETER_KIND_FLAG,
  .value.type  = NYA_TYPE_BOOL,
  .name        = "no-rebuild",
  .description = "Don't rebuild the build system before executing the command.",
};

NYA_ArgParameter help_flag = {
  .kind        = NYA_ARG_PARAMETER_KIND_FLAG,
  .value.type  = NYA_TYPE_BOOL,
  .name        = "help",
  .description = "Show this message.",
};

NYA_ArgCommand run = {
    .name = "run",
    .description = "Run things.",
    .subcommands = {
        &(NYA_ArgCommand){
            .name        = "debug",
            .description = "Run the debug executable.",
            .build_rule  = &run_debug,
        },
        &(NYA_ArgCommand){
            .name        = "release",
            .description = "Run the release executable.",
            .build_rule  = &run_release,
        },
        &(NYA_ArgCommand){
            .name        = "test",
            .description = "Run all tests.",
            .handler     = &test_runner,
            .parameters = { &test_files, },
        },
}};

NYA_ArgCommand build = {
    .name        = "build",
    .description = "Build things.",
    .subcommands = {
        &(NYA_ArgCommand){
            .name        = "debug",
            .description = "Build the debug executable and dll.",
            .build_rule  = &build_project_debug,
        },
        &(NYA_ArgCommand){
            .name        = "debug-exe",
            .description = "Build the debug executable.",
            .build_rule  = &build_project_debug_executable,
        },
        &(NYA_ArgCommand){
            .name        = "debug-dll",
            .description = "Build the debug dll.",
            .build_rule  = &build_project_debug_dll,
        },
        &(NYA_ArgCommand){
            .name        = "release",
            .description = "Build all release executables.",
            .build_rule  = &build_project,
        },
        &(NYA_ArgCommand){
            .name        = "linux",
            .description = "Build the linux release executable.",
            .build_rule  = &build_project_linux_x86_64,
        },
        &(NYA_ArgCommand){
            .name        = "windows",
            .description = "Build the windows release executable.",
            .build_rule  = &build_project_windows_x86_64,
        },
    },
};

NYA_ArgCommand bundle = {
  .name        = "bundle",
  .description = "Bundle for shipping.",
  .build_rule  = &bundle_project,
};

NYA_ArgCommand perf = {
  .name        = "perf",
  .description = "Open Profiler with last profiling data.",
  .build_rule  = &open_perf_report,
};

NYA_ArgCommand docs = {
  .name        = "docs",
  .description = "Open doxygen generated documentation.",
  .build_rule  = &open_docs,
};

NYA_ArgCommand stats = {
  .name        = "stats",
  .description = "Show code statistics.",
  .build_rule  = &show_stats,
};

NYA_ArgCommand update = {
  .name        = "update",
  .description = "Update git submodules.",
  .build_rule  = &update_submodules,
};

NYA_ArgParser parser = {
    .name    = "specturm build system",
    .version = VERSION,

    .root_command = &(NYA_ArgCommand){
        .is_root    = true,
        .parameters = {
            &skip_self_rebuild_flag,
            &help_flag,
        },
        .subcommands = {
            &run,
            &build,
            &bundle,
            &perf,
            &docs,
            &stats,
            &update,
        },
    },
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ENTRY POINT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

s32 main(s32 argc, NYA_CString* argv) {
  parser.executable_name  = argv[0];
  NYA_ArgCommand* command = nya_args_parse_argv(&parser, argc, argv);

  if (!skip_self_rebuild_flag.value.as_bool) nya_rebuild_yourself(&argc, argv, build_rebuild_command);

  if (help_flag.value.as_bool) {
    nya_args_print_usage(&parser, command);
    return EXIT_SUCCESS;
  }

  if (!nya_args_run_command(command)) {
    nya_args_print_usage(&parser, command);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
