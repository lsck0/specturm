#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

// NYA_INTERNAL s32 _nya_job_scheduler(void* data);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_system_job_init(void) {
  NYA_App* app = nya_app_get_instance();

  app->job_system = (NYA_JobSystem){
    .allocator       = nya_arena_create(.name = "job_system_allocator"),
    .job_queue_mutex = SDL_CreateMutex(),
  };

  app->job_system.job_queue = nya_array_create(&app->job_system.allocator, NYA_Job);

  // SDL_Thread* scheduler_thread = SDL_CreateThread(_nya_job_scheduler, "Job Scheduler", nullptr);
  // nya_assert(scheduler_thread != nullptr);
  // SDL_DetachThread(scheduler_thread);

  nya_info("Job system initialized.");
}

void nya_system_job_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  SDL_DestroyMutex(app->job_system.job_queue_mutex);
  nya_array_destroy(&app->job_system.job_queue);

  nya_arena_destroy(&app->job_system.allocator);

  nya_info("Job system deinitialized.");
}

/*
 * ─────────────────────────────────────────────────────────
 * JOB FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_job_submit(job_fn function, void* data, u64 size) {
  nya_assert(function != nullptr);
  nya_assert(data != nullptr);

  NYA_App* app = nya_app_get_instance();

  NYA_Job job = {
    .job  = function,
    .data = data,
    .size = size,
  };

  SDL_LockMutex(app->job_system.job_queue_mutex);
  nya_array_push_back(&app->job_system.job_queue, job);
  SDL_UnlockMutex(app->job_system.job_queue_mutex);
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

// NYA_INTERNAL s32 _nya_job_scheduler(void* data) {
//   nya_unused(data);
//
//   NYA_JobSystem* job_system = &nya_app_get_instance()->job_system;
//
//   for (;;) {
//     printf("Job Scheduler Running...\n");
//     sleep(1);
//   }
// }
