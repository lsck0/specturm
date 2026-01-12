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
  gnyame_setup(argc, argv);
  gnyame_run();
  gnyame_shutdown();

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
#include <unistd.h>

#include "gnyame/gnyame.h"

#define DLL_PATH "./gnyame.debug.so"

void*                     gnyame_dll;
gnyame_setup_fn*          gnyame_setup;
gnyame_run_fn*            gnyame_run;
gnyame_shutdown_fn*       gnyame_shutdown;
gnyame_get_appstate_fn*   gnyame_get_appstate;
gnyame_set_appstate_fn*   gnyame_set_appstate;
gnyame_stop_game_loop_fn* gnyame_stop_game_loop;

u64 gnyame_dll_last_modified;
b8  gnyame_dll_reload_requested = false;

b8    dll_load(void);
void  dll_unload(void);
void* dll_watch_thread_func(void* arg);

s32 main(s32 argc, char** argv) {
  if (!dll_load()) return EXIT_FAILURE;

  pthread_t thread;
  b8        ok = pthread_create(&thread, nullptr, dll_watch_thread_func, nullptr) == 0;
  if (!ok) {
    (void)fprintf(stderr, "Failed to create DLL watch thread\n");
    return EXIT_FAILURE;
  }

  gnyame_setup(argc, argv);

before_run:
  gnyame_run();
  if (gnyame_dll_reload_requested) {
    NYA_App app_state               = gnyame_get_appstate();
    app_state.should_quit_game_loop = false;

    dll_unload();

    ok = dll_load();
    if (!ok) {
      (void)fprintf(stderr, "Failed to reload DLL after change\n");
      return EXIT_FAILURE;
    }

    gnyame_set_appstate(app_state);

    gnyame_dll_reload_requested = false;
    (void)fprintf(stdout, "Reloaded %s successfully\n", DLL_PATH);

    goto before_run;
  }

  gnyame_shutdown();

  dlclose(gnyame_dll);

  return EXIT_SUCCESS;
}

b8 dll_load(void) {
  b8 ok = nya_filesystem_file_last_modified(DLL_PATH, &gnyame_dll_last_modified);
  if (!ok) {
    (void)fprintf(stderr, "Failed to get last modified time for %s\n", DLL_PATH);
    return false;
  }

  gnyame_dll = dlopen(DLL_PATH, RTLD_NOW | RTLD_GLOBAL);
  if (!gnyame_dll) {
    (void)fprintf(stderr, "Failed to load %s: %s\n", DLL_PATH, dlerror());
    return false;
  }

  gnyame_setup          = (gnyame_setup_fn*)dlsym(gnyame_dll, "gnyame_setup");
  gnyame_run            = (gnyame_run_fn*)dlsym(gnyame_dll, "gnyame_run");
  gnyame_shutdown       = (gnyame_shutdown_fn*)dlsym(gnyame_dll, "gnyame_shutdown");
  gnyame_get_appstate   = (gnyame_get_appstate_fn*)dlsym(gnyame_dll, "gnyame_get_appstate");
  gnyame_set_appstate   = (gnyame_set_appstate_fn*)dlsym(gnyame_dll, "gnyame_set_appstate");
  gnyame_stop_game_loop = (gnyame_stop_game_loop_fn*)dlsym(gnyame_dll, "gnyame_stop_game_loop");
  if (!gnyame_setup || !gnyame_run || !gnyame_shutdown || !gnyame_get_appstate || !gnyame_set_appstate ||
      !gnyame_stop_game_loop) {
    (void)fprintf(stderr, "Failed to load symbols from gnyame.debug.so: %s\n", dlerror());
    dlclose(gnyame_dll);
    return false;
  }

  return true;
}

void dll_unload(void) {
  if (gnyame_dll) {
    dlclose(gnyame_dll);

    gnyame_dll            = nullptr;
    gnyame_setup          = nullptr;
    gnyame_run            = nullptr;
    gnyame_shutdown       = nullptr;
    gnyame_get_appstate   = nullptr;
    gnyame_set_appstate   = nullptr;
    gnyame_stop_game_loop = nullptr;
  }
}

void* dll_watch_thread_func(void* arg) {
  nya_unused(arg);

  while (true) {
    u64 last_modified;
    b8  ok = nya_filesystem_file_last_modified(DLL_PATH, &last_modified);
    if (!ok) {
      (void)fprintf(stderr, "Failed to get last modified time for %s\n", DLL_PATH);
      abort();
    }

    if (last_modified != gnyame_dll_last_modified) {
      (void)fprintf(stdout, "Detected change in %s, reloading...\n", DLL_PATH);

      gnyame_dll_reload_requested = true;
      gnyame_stop_game_loop();
    }

    struct timespec ts = {0};
    ts.tv_nsec         = 100UL * 1000UL * 1000UL; // 100 ms
    nanosleep(&ts, nullptr);
  }

  return nullptr;
}

#endif // NYA_IS_DEBUG
