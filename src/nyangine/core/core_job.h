#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_arena.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_JobSystem NYA_JobSystem;

struct NYA_JobSystem {
  NYA_Arena allocator;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_system_job_init(void);
NYA_API NYA_EXTERN void nya_system_job_deinit(void);
