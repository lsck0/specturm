#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_Event nya_event_from_sdl_event(SDL_Event* sdl_event) {
  nya_assert(sdl_event);
  NYA_Event nya_event = {0};

  switch (sdl_event->type) {
    default: break;
  }

  return nya_event;
}

void nya_event_emit(NYA_Event event) {
  nya_unused(event);
}

b8 nya_event_poll(OUT NYA_Event* out_event) {
  nya_assert(out_event);

  *out_event = (NYA_Event){0};

  return false;
}
