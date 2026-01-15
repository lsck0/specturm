#include "nyangine/base/base.h"

#if OS_WINDOWS
#include "nyangine/platform/platform_windows.c"
#elif OS_LINUX
#include "nyangine/platform/platform_linux.c"
#else
#error "Unsupported OS"
#endif

#include "nyangine/base/base_arena.c"
#include "nyangine/base/base_args.c"
#include "nyangine/base/base_build.c"
#include "nyangine/base/base_file.c"
#include "nyangine/base/base_hash.c"
#include "nyangine/base/base_lexer.c"
#include "nyangine/base/base_logging.c"
#include "nyangine/base/base_random.c"
#include "nyangine/base/base_string.c"
#include "nyangine/core/core_app.c"
#include "nyangine/core/core_entity.c"
#include "nyangine/core/core_event.c"
#include "nyangine/core/core_perf.c"
#include "nyangine/core/core_window.c"
#include "nyangine/math/math_la.c"
