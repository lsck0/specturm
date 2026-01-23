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
    .allocator_mutex = SDL_CreateMutex(),
    .allocator       = nya_arena_create(.name = "job_system_allocator"),
  };

  app->job_system.job_queue = nya_array_create(&app->job_system.allocator, NYA_Job);

  // SDL_Thread* scheduler_thread = SDL_CreateThread(_nya_job_scheduler, "Job Scheduler", nullptr);
  // nya_assert(scheduler_thread);
  // SDL_DetachThread(scheduler_thread);
}

void nya_system_job_deinit(void) {
  NYA_App* app = nya_app_get_instance();

  nya_array_destroy(&app->job_system.job_queue);

  SDL_DestroyMutex(app->job_system.allocator_mutex);
  nya_arena_destroy(&app->job_system.allocator);
}

/*
 * ─────────────────────────────────────────────────────────
 * JOB FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

void nya_job_submit(job_fn function, void* data) {
  nya_assert(function);
  nya_assert(data);

  NYA_App* app = nya_app_get_instance();

  // TODO: Is the data still valid when the job runs?
  // How/Who cleans it up?
  // should we make the arena allocator thread safe?
  NYA_Job job = {
    .job  = function,
    .data = data,
  };

  SDL_LockMutex(app->job_system.allocator_mutex);
  nya_array_push_back(&app->job_system.job_queue, job);
  SDL_UnlockMutex(app->job_system.allocator_mutex);
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
