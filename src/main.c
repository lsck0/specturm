#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * RELEASE ENTRY POINT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if !NYA_IS_DEBUG
#include "gnyame/gnyame.c"
#include "gnyame/gnyame.h"

s32 main(s32 argc, char** argv) {
  gnyame_init(argc, argv);
  gnyame_run();
  gnyame_deinit();

  return EXIT_SUCCESS;
}
#endif // !NYA_IS_DEBUG

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * DEBUG ENTRY POINT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if NYA_IS_DEBUG
#include <dlfcn.h>
#include <pthread.h>
#include <sys/stat.h>

#define DLL_PATH "./gnyame.debug.so"

typedef void(gnyame_init_fn)(s32 argc, char** argv);
typedef void(gnyame_run_fn)(void);
typedef void(gnyame_deinit_fn)(void);

NYA_App*          nya_app;
void*             gnyame_dll;
gnyame_init_fn*   gnyame_init;
gnyame_run_fn*    gnyame_run;
gnyame_deinit_fn* gnyame_deinit;
u64               gnyame_dll_last_modified;
atomic b8         gnyame_dll_reload_requested = false;

b8    dll_load(void);
b8    dll_unload(void);
void* dll_watch_thread_func(void* arg);

s32 main(s32 argc, char** argv) {
  if (!dll_load()) return EXIT_FAILURE;

  pthread_t thread;
  b8        ok = pthread_create(&thread, nullptr, dll_watch_thread_func, nullptr) == 0;
  if (!ok) {
    (void)fprintf(stderr, "Failed to create DLL watch thread.\n");
    return EXIT_FAILURE;
  }

  gnyame_init(argc, argv);
  nya_app = nya_app_get_instance();

before_run:
  gnyame_run();
  if (gnyame_dll_reload_requested) {
    ok = dll_unload();
    if (!ok) {
      (void)fprintf(stderr, "Failed to unload DLL before reload.\n");
      return EXIT_FAILURE;
    }

    // give the compiler time to finish writing the new DLL
    struct timespec ts = {0};
    ts.tv_nsec         = 150UL * 1000UL * 1000UL; // 150 ms
    nanosleep(&ts, nullptr);

    ok = dll_load();
    if (!ok) {
      (void)fprintf(stderr, "Failed to reload DLL after change.\n");
      return EXIT_FAILURE;
    }

    gnyame_dll_reload_requested    = false;
    nya_app->should_quit_game_loop = false;
    (void)fprintf(stdout, "Reloaded %s successfully.\n", DLL_PATH);

    goto before_run;
  }

  gnyame_deinit();

  dlclose(gnyame_dll);

  return EXIT_SUCCESS;
}

b8 dll_load(void) {
  gnyame_dll = dlopen(DLL_PATH, RTLD_NOW | RTLD_GLOBAL);
  if (!gnyame_dll) {
    (void)fprintf(stderr, "Failed to load %s: %s.\n", DLL_PATH, dlerror());
    return false;
  }

  gnyame_init   = (gnyame_init_fn*)dlsym(gnyame_dll, "gnyame_init");
  gnyame_run    = (gnyame_run_fn*)dlsym(gnyame_dll, "gnyame_run");
  gnyame_deinit = (gnyame_deinit_fn*)dlsym(gnyame_dll, "gnyame_deinit");
  if (!gnyame_init || !gnyame_run || !gnyame_deinit) {
    (void)fprintf(stderr, "Failed to load symbols from %s: %s.\n", DLL_PATH, dlerror());
    dlclose(gnyame_dll);
    return false;
  }

  b8 ok = nya_filesystem_last_modified(DLL_PATH, &gnyame_dll_last_modified);
  if (!ok) {
    (void)fprintf(stderr, "Failed to get last modified time for %s.\n", DLL_PATH);
    dlclose(gnyame_dll);
    return false;
  }

  return true;
}

b8 dll_unload(void) {
  if (gnyame_dll) {
    b8 ok = dlclose(gnyame_dll) == 0;
    if (!ok) {
      (void)fprintf(stderr, "Failed to unload %s: %s.\n", DLL_PATH, dlerror());
      return false;
    }

    gnyame_dll    = nullptr;
    gnyame_init   = nullptr;
    gnyame_run    = nullptr;
    gnyame_deinit = nullptr;
  }

  return true;
}

void* dll_watch_thread_func(void* arg) {
  nya_unused(arg);

  while (true) {
    u64 last_modified;
    b8  ok = nya_filesystem_last_modified(DLL_PATH, &last_modified);
    if (!ok) continue;

    if (last_modified != gnyame_dll_last_modified && !gnyame_dll_reload_requested) {
      (void)fprintf(stdout, "Detected change in %s, reloading soon...\n", DLL_PATH);

      gnyame_dll_reload_requested    = true;
      nya_app->should_quit_game_loop = true;
    }

    struct timespec ts = {0};
    ts.tv_nsec         = 50UL * 1000UL * 1000UL; // 50 ms
    nanosleep(&ts, nullptr);
  }

  return nullptr;
}

#endif // NYA_IS_DEBUG
