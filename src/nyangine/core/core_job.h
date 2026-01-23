#pragma once

// TODO: abandoned until the rest of the code is threadsafe.

#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_thread.h"

#include "nyangine/base/base.h"
#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_JobSystem NYA_JobSystem;
typedef struct NYA_Job       NYA_Job;
typedef SDL_Thread*          SDL_ThreadPtr;
nya_derive_array(NYA_Job);
nya_derive_array(SDL_ThreadPtr);

typedef void (*job_fn)(void* data);

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_JobSystem {
  SDL_Mutex* allocator_mutex;
  NYA_Arena  allocator;

  NYA_JobArray job_queue;
};

/*
 * ─────────────────────────────────────────────────────────
 * JOB STRUCTS
 * ─────────────────────────────────────────────────────────
 */

struct NYA_Job {
  job_fn job;
  void*  data;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_system_job_init(void);
NYA_API NYA_EXTERN void nya_system_job_deinit(void);

/*
 * ─────────────────────────────────────────────────────────
 * JOB FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN void nya_job_submit(job_fn function, void* data);
