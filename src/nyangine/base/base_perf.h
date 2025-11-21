#pragma once

#include "nyangine/base/base_string.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPE DEFINITIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define NYA_PERF_MEASUREMENT_SAMPLES 64

typedef struct NYA_PerfMeasurement NYA_PerfMeasurement;
nya_derive_array(NYA_PerfMeasurement);

/**
 * The arrays wrap, so to print in order:
 * for (u64 i = measurement->current + 1; i != measurement->current; i = (i + 1) % NYA_PERF_MEASUREMENT_SAMPLES) {}
 */
struct NYA_PerfMeasurement {
  NYA_ConstCString name;
  b8               is_running;
  u64              started[NYA_PERF_MEASUREMENT_SAMPLES];
  u64              ended[NYA_PERF_MEASUREMENT_SAMPLES];
  u64              elapsed_ms[NYA_PERF_MEASUREMENT_SAMPLES];
  u64              current;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS AND MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if NYA_IS_DEBUG
#define nya_perf_timer_get(name)   _nya_perf_timer_get(name)
#define nya_perf_timer_start(name) _nya_perf_timer_start(name)
#define nya_perf_timer_stop(name)  _nya_perf_timer_stop(name)
#define nya_perf_timer_get_all()   _nya_perf_timer_get_all()
#else
#define nya_perf_get_timer(name)                                                                                       \
  ({                                                                                                                   \
    nya_unused(name);                                                                                                  \
    nya_panic("Perf module is disabled.");                                                                             \
  })
#define nya_perf_timer_start(name) nya_unused(name)
#define nya_perf_timer_stop(name)  nya_unused(name)
#define nya_perf_timer_get_all()   nya_panic("Perf module is disabled.")
#endif // NYA_IS_DEBUG

// clang-format off
#define nya_perf_time_this_scope(name) __attr_cleanup(_nya_perf_cleanup) NYA_CString _nya_perf_scope_timer_##__LINE__ = (nya_perf_timer_start(name), (NYA_CString)(name))
#define nya_perf_time_this_function()  nya_perf_time_this_scope(__FILE__##__FUNCTION__)
// clang-format on

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * INTERNALS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if NYA_IS_DEBUG
NYA_API NYA_EXTERN NYA_PerfMeasurement*      _nya_perf_timer_get(NYA_ConstCString name);
NYA_API NYA_EXTERN void                      _nya_perf_timer_start(NYA_ConstCString name);
NYA_API NYA_EXTERN void                      _nya_perf_timer_stop(NYA_ConstCString name);
NYA_API NYA_EXTERN void                      _nya_perf_timer_reset(NYA_ConstCString name);
NYA_API NYA_EXTERN NYA_PerfMeasurementArray* _nya_perf_timer_get_all(void);
#endif // NYA_IS_DEBUG

NYA_API NYA_EXTERN inline void _nya_perf_cleanup(NYA_CString* name_ptr) {
  if (name_ptr && *name_ptr) nya_perf_timer_stop(*name_ptr);
}
