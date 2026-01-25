#include "nyangine/base/base.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * RELEASE ENTRY POINT
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if !NYA_IS_DEBUG
#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

#include "gnyame/gnyame.c"
#include "gnyame/gnyame.h"

s32 main(s32 argc, NYA_CString* argv) {
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

#define NYA_ASSET_MANIFEST_ALREADY_INCLUDED
#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

#define DLL_PATH "./gnyame.debug.so"
typedef void(gnyame_init_fn)(s32 argc, NYA_CString* argv);
typedef void(gnyame_run_fn)(void);
typedef void(gnyame_deinit_fn)(void);

NYA_App*          nya_app                             = nullptr;
void*             gnyame_dll                          = nullptr;
gnyame_init_fn*   gnyame_init                         = nullptr;
gnyame_run_fn*    gnyame_run                          = nullptr;
gnyame_deinit_fn* gnyame_deinit                       = nullptr;
atomic u64        gnyame_dll_last_modified            = 0;
atomic b8         gnyame_dll_reload_requested         = false;
atomic b8         gnyame_dll_watch_thread_should_exit = false;

void  dll_load(void);
void  dll_unload(void);
void* dll_watch_thread_fn(void* arg);

s32 main(s32 argc, NYA_CString* argv) {
  b8 ok;

  dll_load();

  pthread_t thread;
  ok = pthread_create(&thread, nullptr, dll_watch_thread_fn, nullptr) == 0;
  nya_assert(ok, "Failed to create DLL watch thread.");

  gnyame_init(argc, argv);
  nya_app = nya_app_get_instance();

  while (!nya_app->should_quit_game_loop) {
    gnyame_run();

    if (gnyame_dll_reload_requested) {
      dll_unload();

      // give the compiler time to finish writing the new DLL
      struct timespec ts = { 0 };
      ts.tv_nsec         = 150UL * 1000UL * 1000UL; // 150 ms
      nanosleep(&ts, nullptr);

      dll_load();

      gnyame_dll_reload_requested    = false;
      nya_app->should_quit_game_loop = false;
      nya_debug("Reloaded %s.", DLL_PATH);
    }
  }

  gnyame_deinit();

  gnyame_dll_watch_thread_should_exit = true;
  ok                                  = pthread_join(thread, nullptr) == 0;
  nya_assert(ok, "Failed to join DLL watch thread.");

  dll_unload();

  return EXIT_SUCCESS;
}

void dll_load(void) {
  gnyame_dll = dlopen(DLL_PATH, RTLD_NOW | RTLD_GLOBAL);
  nya_assert(gnyame_dll, "Failed to load %s: %s.", DLL_PATH, dlerror());

  gnyame_init   = (gnyame_init_fn*)dlsym(gnyame_dll, "gnyame_init");
  gnyame_run    = (gnyame_run_fn*)dlsym(gnyame_dll, "gnyame_run");
  gnyame_deinit = (gnyame_deinit_fn*)dlsym(gnyame_dll, "gnyame_deinit");
  nya_assert(gnyame_init && gnyame_run && gnyame_deinit, "Failed to load symbols from %s: %s.", DLL_PATH, dlerror());

  u64 gnyame_dll_last_modified_temp;
  b8  ok = nya_filesystem_last_modified(DLL_PATH, &gnyame_dll_last_modified_temp);
  nya_assert(ok, "Failed to get last modified time for %s.", DLL_PATH);
  gnyame_dll_last_modified = gnyame_dll_last_modified_temp;
}

void dll_unload(void) {
  if (!gnyame_dll) return;

  b8 ok = dlclose(gnyame_dll) == 0;
  nya_assert(ok, "Failed to unload %s: %s.", DLL_PATH, dlerror());

  gnyame_dll    = nullptr;
  gnyame_init   = nullptr;
  gnyame_run    = nullptr;
  gnyame_deinit = nullptr;
}

void* dll_watch_thread_fn(void* arg) {
  nya_unused(arg);

  while (!gnyame_dll_watch_thread_should_exit) {
    u64 last_modified;
    b8  ok = nya_filesystem_last_modified(DLL_PATH, &last_modified);

    if (ok && last_modified != gnyame_dll_last_modified && !gnyame_dll_reload_requested) {
      nya_debug("%s was changed, requesting reload.", DLL_PATH);
      gnyame_dll_reload_requested    = true;
      nya_app->should_quit_game_loop = true;
    }

    struct timespec ts = { 0 };
    ts.tv_nsec         = 50UL * 1000UL * 1000UL; // 50 ms
    nanosleep(&ts, nullptr);
  }

  return nullptr;
}

#endif // NYA_IS_DEBUG
