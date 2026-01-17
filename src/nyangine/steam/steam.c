#include "nyangine/steam/steam.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * STEAM SDK FLAT API (C-compatible exports from steam_api library)
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef s32  ESteamAPIInitResult;
typedef char SteamErrMsg[1024];

extern ESteamAPIInitResult SteamAPI_InitFlat(SteamErrMsg* pOutErrMsg);
extern void                SteamAPI_Shutdown(void);
extern void                SteamAPI_RunCallbacks(void);
extern b8                  SteamAPI_IsSteamRunning(void);
extern b8                  SteamAPI_RestartAppIfNecessary(u32 unOwnAppID);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_SteamInitResult nya_steam_init(OUT NYA_CString err_msg) {
  SteamErrMsg         raw_err = {0};
  ESteamAPIInitResult result  = SteamAPI_InitFlat(&raw_err);

  if (err_msg != nullptr && result != NYA_STEAM_INIT_OK) {
    for (s32 i = 0; i < 1024 && raw_err[i] != '\0'; i++) err_msg[i] = raw_err[i];
  }

  return (NYA_SteamInitResult)result;
}

void nya_steam_shutdown(void) {
  SteamAPI_Shutdown();
}

b8 nya_steam_is_running(void) {
  return SteamAPI_IsSteamRunning();
}

void nya_steam_run_callbacks(void) {
  SteamAPI_RunCallbacks();
}

b8 nya_steam_restart_app_if_necessary(u32 app_id) {
  return SteamAPI_RestartAppIfNecessary(app_id);
}
