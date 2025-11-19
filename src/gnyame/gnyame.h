#pragma once

#include "nyangine/nyangine.h"

NYA_API NYA_EXTERN void gnyame_setup(s32 argc, char** argv);
NYA_API NYA_EXTERN void gnyame_run(void);
NYA_API NYA_EXTERN void gnyame_shutdown(void);

#if NYA_IS_DEBUG
NYA_API NYA_EXTERN NYA_App gnyame_get_appstate();
NYA_API NYA_EXTERN void    gnyame_set_appstate(NYA_App app);
NYA_API NYA_EXTERN void    gnyame_stop_game_loop(void);
#endif // NYA_IS_DEBUG
