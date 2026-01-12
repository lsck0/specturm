#pragma once

#include "nyangine/nyangine.h"

#if !NYA_IS_DEBUG
NYA_API NYA_EXTERN void gnyame_setup(s32 argc, char** argv);
NYA_API NYA_EXTERN void gnyame_run(void);
NYA_API NYA_EXTERN void gnyame_shutdown(void);
#else
typedef void(gnyame_setup_fn)(s32 argc, char** argv);
typedef void(gnyame_run_fn)(void);
typedef void(gnyame_shutdown_fn)(void);
typedef NYA_App(gnyame_get_appstate_fn)(void);
typedef void(gnyame_set_appstate_fn)(NYA_App app);
typedef void(gnyame_stop_game_loop_fn)(void);
#endif // !NYA_IS_DEBUG
