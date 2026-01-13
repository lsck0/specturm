#pragma once

#include "nyangine/nyangine.h"

// loaded dynamically in debug builds
#if !NYA_IS_DEBUG
NYA_API NYA_EXTERN void gnyame_setup(s32 argc, char** argv);
NYA_API NYA_EXTERN void gnyame_run(void);
NYA_API NYA_EXTERN void gnyame_shutdown(void);
#endif // !NYA_IS_DEBUG
