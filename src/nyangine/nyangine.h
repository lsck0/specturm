/* !
 * Compilation options:
 * -DVERSION="<version_string>" : Set the engine version string.
 * -DGIT_COMMIT="<git_commit_hash>" : Set the git commit hash.
 * -DNYA_NO_ASSERT : Disable runtime assertions.
 * -DNYA_ARENA_FORCE_DEBUG : Force arena to use debug mode, to enable it in release builds.
 * -DNYA_ARENA_FORCE_NODEBUG : Force arena to use no-debug mode, to disable it in debug builds.
 * */

#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_assert.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_bits.h"
#include "nyangine/base/base_dll.h"
#include "nyangine/base/base_ints.h"
#include "nyangine/base/base_logging.h"
#include "nyangine/base/base_math.h"
#include "nyangine/base/base_memory.h"
