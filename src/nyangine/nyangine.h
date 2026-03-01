/**
 * @file nyangine.h
 *
 * Compilation options:
 * -DDEBUG=<true/false> : Compile in debug mode.
 * -DVERSION="<version_string>" : Set the engine version string.
 * -DGIT_COMMIT="<git_commit_hash>" : Set the git commit hash.
 * -DNYA_NO_ASSERT : Disable runtime assertions.
 * -DNYA_ARENA_FORCE_DEBUG : Force arena to use debug mode, to enable it in release builds.
 * -DNYA_ARENA_FORCE_NODEBUG : Force arena to use no-debug mode, to disable it in debug builds.
 * -DNYA_ASSET_BACKEND_FS : Use filesystem as the asset backend, comes with hot-reloading.
 * -DNYA_ASSET_BACKEND_BLOB : Use embedded blob as the asset backend.
 * */

#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_args.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_assert.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_base64.h"
#include "nyangine/base/base_bits.h"
#include "nyangine/base/base_build.h"
#include "nyangine/base/base_color.h"
#include "nyangine/base/base_crc.h"
#include "nyangine/base/base_dict.h"
#include "nyangine/base/base_dll.h"
#include "nyangine/base/base_error.h"
#include "nyangine/base/base_file.h"
#include "nyangine/base/base_guard.h"
#include "nyangine/base/base_hash.h"
#include "nyangine/base/base_heap.h"
#include "nyangine/base/base_hmap.h"
#include "nyangine/base/base_hset.h"
#include "nyangine/base/base_integrity.h"
#include "nyangine/base/base_keys.h"
#include "nyangine/base/base_lexer.h"
#include "nyangine/base/base_logging.h"
#include "nyangine/base/base_memory.h"
#include "nyangine/base/base_mouse.h"
#include "nyangine/base/base_noise.h"
#include "nyangine/base/base_object.h"
#include "nyangine/base/base_perf.h"
#include "nyangine/base/base_random.h"
#include "nyangine/base/base_ring.h"
#include "nyangine/base/base_string.h"
#include "nyangine/base/base_tween.h"
#include "nyangine/base/base_types.h"
#include "nyangine/base/base_uuid.h"
#include "nyangine/core/core_app.h"
#include "nyangine/core/core_asset.h"
#include "nyangine/core/core_callback.h"
#include "nyangine/core/core_entity.h"
#include "nyangine/core/core_event.h"
#include "nyangine/core/core_input.h"
#include "nyangine/core/core_job.h"
#include "nyangine/core/core_window.h"
#include "nyangine/math/math.h"
#include "nyangine/nn/nn.h"
#include "nyangine/platform/clock/clock.h"
#include "nyangine/platform/command/command.h"
#include "nyangine/platform/filesystem/filesystem.h"
#include "nyangine/platform/signals/signals.h"
#include "nyangine/renderer/renderer.h"
#include "nyangine/steam/steam.h"
#include "nyangine/ui/ui.h"
