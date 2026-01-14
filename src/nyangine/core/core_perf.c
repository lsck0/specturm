#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

__attr_unused NYA_INTERNAL NYA_Arena                _nya_perf_arena         = {0};
__attr_unused NYA_INTERNAL NYA_PerfMeasurementArray _nya_perf_measurements  = {0};
__attr_unused NYA_INTERNAL u64                      _nya_perf_start_time_ms = 0;

__attr_unused NYA_INTERNAL void _nya_perf_init(void);
__attr_unused NYA_INTERNAL void _nya_perf_shutdown(void);
__attr_unused NYA_INTERNAL u64  _nya_perf_time_since_start_ms(void);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_PerfMeasurement* _nya_perf_timer_get(NYA_ConstCString name) {
  nya_assert(name);

  nya_array_foreach (&_nya_perf_measurements, measurement) {
    if (nya_string_equals(measurement->name, name)) return measurement;
  }

  return nullptr;
}

void _nya_perf_timer_start(NYA_ConstCString name) {
  nya_assert(name);

  NYA_PerfMeasurement* measurement = _nya_perf_timer_get(name);
  if (measurement != nullptr) {
    u64 index                      = (measurement->current + 1) % NYA_PERF_MEASUREMENT_SAMPLES;
    measurement->is_running        = true;
    measurement->started[index]    = _nya_perf_time_since_start_ms();
    measurement->ended[index]      = 0;
    measurement->elapsed_ms[index] = 0;
    measurement->current           = index;
    return;
  }

  NYA_PerfMeasurement new_measurement = {
      .name       = name,
      .is_running = true,
      .current    = 0,
      .started    = {[0] = _nya_perf_time_since_start_ms()},
      .ended      = {0},
      .elapsed_ms = {0},
  };
  nya_array_push_back(&_nya_perf_measurements, new_measurement);
}

void _nya_perf_timer_stop(NYA_ConstCString name) {
  nya_assert(name);

  NYA_PerfMeasurement* measurement = _nya_perf_timer_get(name);
  nya_assert(measurement != nullptr, "Timer '%s' was not started.", name);

  u64 index                      = measurement->current;
  measurement->is_running        = false;
  measurement->ended[index]      = _nya_perf_time_since_start_ms();
  measurement->elapsed_ms[index] = measurement->ended[index] - measurement->started[index];
}

NYA_PerfMeasurementArray* _nya_perf_timer_get_all(void) {
  return &_nya_perf_measurements;
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if NYA_IS_DEBUG
__attr_constructor NYA_INTERNAL void _nya_perf_init(void) {
  // nya_assert(nya_is_zeroed(_nya_perf_arena));
  nya_assert(nya_is_zeroed(_nya_perf_measurements));

  _nya_perf_arena        = nya_arena_new();
  _nya_perf_measurements = nya_array_new(&_nya_perf_arena, NYA_PerfMeasurement);

  _nya_perf_start_time_ms = nya_clock_get_timestamp_ms();
}

__attr_destructor NYA_INTERNAL void _nya_perf_shutdown(void) {
  nya_arena_destroy(&_nya_perf_arena);
}
#endif // NYA_IS_DEBUG

NYA_INTERNAL u64 _nya_perf_time_since_start_ms(void) {
  return nya_clock_get_timestamp_ms() - _nya_perf_start_time_ms;
}
