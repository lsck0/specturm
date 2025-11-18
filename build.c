// TODO: switch from nob to the nyangine stdlib
// #include "./src/nyangine/nyangine.c"
// #include "./src/nyangine/nyangine.h"

#define NOB_IMPLEMENTATION
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "./vendor/nob/nob.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * Configuration
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define PROJECT_NAME "gnyame"
#define PROJECT_PATH "./src/gnyame/main.c"
#define VERSION      "0.0.0"

#define DEBUG_BINARY          PROJECT_NAME "." VERSION ".debug"
#define WINDOWS_X86_64_BINARY PROJECT_NAME "." VERSION ".windows-x86_64.exe"
#define LINUX_X86_64_BINARY   PROJECT_NAME "." VERSION ".linux-x86_64"

// clang-format off
#define CC            "clang"
#define CFLAGS        "-std=c23", "-ggdb", "-fenable-matrix", "-DVERSION=" VERSION, nob_temp_sprintf("-DGIT_COMMIT=%s", GIT_COMMIT)
#define WARNINGS      "-pedantic", "-Wall", "-Wextra", "-Wpedantic", "-Wno-gnu", "-Wno-gcc-compat", "-Wno-initializer-overrides", "-Wno-keyword-macro"
#define INCLUDE_PATHS "-I./src/", "-I./vendor/sdl/include/"
#define LINKER_FLAGS  "-lm", "-pthread", "-lSDL3"
#define NPROCS        16

#define FLAGS_DEBUG   "-O0", "-DIS_DEBUG=true", "-fno-omit-frame-pointer", "-fno-optimize-sibling-calls", "-fno-sanitize-recover=all", "-fsanitize=address,leak,undefined,signed-integer-overflow,unsigned-integer-overflow,shift,float-cast-overflow,float-divide-by-zero,pointer-overflow"
#define FLAGS_RELEASE "-O3", "-D_FORTIFY_SOURCE=2", "-fno-omit-frame-pointer", "-fstack-protector-strong"

#define FLAGS_WINDOWS_X86_64 "--target=x86_64-w64-mingw32", "-Wl,-subsystem,windows", "-static", "-L./vendor/sdl/build-window-x86_64/", "-lcomdlg32", "-ldxguid", "-lgdi32", "-limm32", "-lkernel32", "-lole32", "-loleaut32", "-lsetupapi", "-luser32", "-luuid", "-lversion", "-lwinmm"
#define FLAGS_LINUX_X86_64   "-L./vendor/sdl/build-linux-x86_64/"
// clang-format on

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * Main
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

static void build_debug(void);
static void build_release(void);
static void build_docs(void);
static void run_debug(void);
static void run_tests(void);
static void run_stats(void);

static void build_windows_x86_64(void);
static void build_linux_x86_64(void);

static void build_shaders(void);
static void build_assets(void);

static void build_windows_x86_64_dependencies(void);
static void build_linux_x86_64_dependencies(void);

static Nob_Cmd     CMD        = {};
static const char* GIT_COMMIT = "unknown";

void usage_and_exit(const char* prog_name) {
  printf("Usage: %s [task]\n", prog_name);
  printf("Tasks:\n");
  printf("  run:debug\n");
  printf("  run:tests\n");
  printf("  run:stats\n");
  printf("  build:debug\n");
  printf("  build:release\n");
  printf("  build:docs\n");

  exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  // set git hash
  Nob_String_Builder sb = {0};
  if (nob_read_entire_file("./.git/refs/heads/master", &sb)) {
    GIT_COMMIT = nob_temp_sv_to_cstr(nob_sv_from_parts(sb.items, sb.count - 1));
  }

  if (argc != 2) usage_and_exit(argv[0]);

  const char* task = argv[1];
  if (memcmp(task, "build:debug", 11) == 0) {
    build_debug();
  } else if (memcmp(task, "build:release", 13) == 0) {
    build_release();
  } else if (memcmp(task, "build:docs", 10) == 0) {
    build_docs();
  } else if (memcmp(task, "run:debug", 9) == 0) {
    run_debug();
  } else if (memcmp(task, "run:tests", 9) == 0) {
    run_tests();
  } else if (memcmp(task, "run:stats", 9) == 0) {
    run_stats();
  } else {
    usage_and_exit(argv[0]);
  }

  return EXIT_SUCCESS;
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * Tasks
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

static void build_release(void) {
  build_windows_x86_64();
  build_linux_x86_64();
}

static void build_docs(void) {
  nob_cmd_append(&CMD, "doxygen", "./docs/doxygen.config");
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);

  nob_cmd_append(&CMD, "xdg-open", "./docs/doxygen/html/index.html");
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
}

static void run_debug(void) {
  build_debug();

  setenv("ASAN_OPTIONS", "detect_leaks=1:strict_string_checks=1:halt_on_error=1", 1);
  setenv("LSAN_OPTIONS", "suppressions=lsan.supp", 1);
  setenv("UBSAN_OPTIONS", "print_stacktrace=1:halt_on_error=1", 1);
  nob_cmd_append(&CMD, "./" DEBUG_BINARY);
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
}

static void run_tests(void) {
  nob_cmd_append(&CMD, "find", "./tests", "-name", "*.c");
  if (!nob_cmd_run(&CMD, .stdout_path = "./tests/manifest.txt")) exit(EXIT_FAILURE);

  // TODO: this will be easier with nya_string*
}

static void run_stats(void) {
  nob_cmd_append(&CMD, "tokei", ".", "--exclude", "vendor");
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * Builds
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

static void build_debug(void) {
  build_linux_x86_64_dependencies();
  build_assets();

  nob_cmd_append(
      &CMD,
      CC,
      PROJECT_PATH,
      "-o",
      DEBUG_BINARY,
      CFLAGS,
      WARNINGS,
      INCLUDE_PATHS,
      LINKER_FLAGS,
      FLAGS_DEBUG,
      FLAGS_LINUX_X86_64,
  );
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
}

static void build_windows_x86_64(void) {
  build_windows_x86_64_dependencies();
  build_assets();

  // icon resource
  nob_cmd_append(&CMD, "x86_64-w64-mingw32-windres", "./assets/icon.rc", "-O", "coff", "-o", "./assets/icon.res");
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);

  nob_cmd_append(
      &CMD,
      CC,
      PROJECT_PATH,
      "-o",
      WINDOWS_X86_64_BINARY,
      CFLAGS,
      WARNINGS,
      INCLUDE_PATHS,
      LINKER_FLAGS,
      FLAGS_RELEASE,
      FLAGS_WINDOWS_X86_64,
      "./assets/icon.res"
  );
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
}

static void build_linux_x86_64(void) {
  build_linux_x86_64_dependencies();
  build_assets();

  nob_cmd_append(
      &CMD,
      CC,
      PROJECT_PATH,
      "-o",
      LINUX_X86_64_BINARY,
      CFLAGS,
      WARNINGS,
      INCLUDE_PATHS,
      LINKER_FLAGS,
      FLAGS_RELEASE,
      FLAGS_LINUX_X86_64,
  );
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * Assets
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

static void build_shaders(void) {
  static const char* shader_compiler = "./vendor/sdl-shadercross/build/shadercross";

  if (!nob_file_exists(shader_compiler)) {
    nob_cmd_append(
        &CMD,
        "cmake",
        "-S",
        "./vendor/sdl-shadercross",
        "-B",
        "./vendor/sdl-shadercross/build",
        "-GNinja",
        "-DSDLSHADERCROSS_VENDORED=ON"
    );
    if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);

    nob_cmd_append(
        &CMD,
        "cmake",
        "--build",
        "./vendor/sdl-shadercross/build",
        "--config",
        "Release",
        "--",
        nob_temp_sprintf("-j %d", NPROCS)
    );
    if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
  }

  Nob_File_Paths shaders     = {0};
  const char*    shader_path = "./assets/shaders/source/";
  nob_read_entire_dir(shader_path, &shaders);

  // [.] [..] file1 ... fileN
  // -> fileN [..] file1 ... fileN-1
  // -> fileN-1 fileN file1 ... fileN-2
  nob_da_remove_unordered(&shaders, 0);
  nob_da_remove_unordered(&shaders, 1);

  nob_cmd_append(&CMD, "mkdir", "-p", "./assets/shaders/compiled/DXIL/");
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
  nob_cmd_append(&CMD, "mkdir", "-p", "./assets/shaders/compiled/SPIRV/");
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
  nob_cmd_append(&CMD, "mkdir", "-p", "./assets/shaders/compiled/MSL/");
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);

  nob_da_foreach(const char*, shader, &shaders) {
    // remove extension
    char* dot = strstr(*shader, ".hlsl");
    if (dot) *dot = '\0';

    {
      const char* output_path = nob_temp_sprintf("./assets/shaders/compiled/DXIL/%s.dxil", *shader);
      const char* input_path  = nob_temp_sprintf("%s%s.hlsl", shader_path, *shader);
      nob_cmd_append(&CMD, shader_compiler, input_path, "-o", output_path, "-s", "hlsl", "-d", "dxil");
      if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
    }

    {
      const char* output_path = nob_temp_sprintf("./assets/shaders/compiled/SPIRV/%s.spv", *shader);
      const char* input_path  = nob_temp_sprintf("%s%s.hlsl", shader_path, *shader);
      nob_cmd_append(&CMD, shader_compiler, input_path, "-o", output_path, "-s", "hlsl", "-d", "spirv");
      if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
    }

    {
      const char* output_path = nob_temp_sprintf("./assets/shaders/compiled/MSL/%s.msl", *shader);
      const char* input_path  = nob_temp_sprintf("%s%s.hlsl", shader_path, *shader);
      nob_cmd_append(&CMD, shader_compiler, input_path, "-o", output_path, "-s", "hlsl", "-d", "msl");
      if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
    }
  }
}

static void build_assets(void) {
  build_shaders();

  // TODO: asset embedding. probably: .png, .wav, ... ---> assets.c + assets.h
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * Dependencies
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

static void build_windows_x86_64_dependencies(void) {
  static const char* sdl = "./vendor/sdl/build-window-x86_64/";

  struct stat st;
  if (stat(sdl, &st) == 0 && S_ISDIR(st.st_mode)) return;

  nob_cmd_append(
      &CMD,
      "cmake",
      "-S",
      "./vendor/sdl",
      "-B",
      sdl,
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
      "-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=BOTH"
  );
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);

  nob_cmd_append(&CMD, "cmake", "--build", sdl, "--config", "Release", "--", nob_temp_sprintf("-j %d", NPROCS));
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
}

static void build_linux_x86_64_dependencies(void) {
  static const char* sdl = "./vendor/sdl/build-linux-x86_64/";

  struct stat st;
  if (stat(sdl, &st) == 0 && S_ISDIR(st.st_mode)) return;

  nob_cmd_append(
      &CMD,
      "cmake",
      "-S",
      "./vendor/sdl",
      "-B",
      sdl,
      "-GNinja",
      "-DCMAKE_BUILD_TYPE=Release",
      "-DCMAKE_POSITION_INDEPENDENT_CODE=ON",
      "-DSDL_SHARED=OFF",
      "-DSDL_STATIC=ON"
  );
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);

  nob_cmd_append(&CMD, "cmake", "--build", sdl, "--config", "Release", "--", nob_temp_sprintf("-j %d", NPROCS));
  if (!nob_cmd_run(&CMD)) exit(EXIT_FAILURE);
}
