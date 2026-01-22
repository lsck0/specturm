#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_hmap.h"
#include "nyangine/base/base_hset.h"
#include "nyangine/base/base_string.h"
#include "nyangine/core/core_event.h"

#ifndef NYA_ASSET_MANIFEST_ALREADY_INCLUDED
#include "../../../assets/assets.h"
#endif // NYA_ASSET_MANIFEST_ALREADY_INCLUDED

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_Asset           NYA_Asset;
typedef struct NYA_AssetBlobHeader NYA_AssetBlobHeader;
typedef struct NYA_AssetHandle     NYA_AssetHandle;
typedef struct NYA_AssetSystem     NYA_AssetSystem;
nya_derive_hmap(NYA_CString, NYA_Asset);

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_AssetSystem {
  NYA_Arena allocator;

  NYA_CString_NYA_Asset_HMap assets;
  NYA_CStringArray           changed_assets;
};

/*
 * ─────────────────────────────────────────────────────────
 * ASSET STRUCTS
 * ─────────────────────────────────────────────────────────
 */

typedef enum {
  NYA_ASSET_STATUS_UNKNOWN = 0,
  NYA_ASSET_STATUS_UNLOADED,
  NYA_ASSET_STATUS_LOADING,
  NYA_ASSET_STATUS_LOADED,
  NYA_ASSET_STATUS_COUNT,
} NYA_AssetStatus;

struct NYA_AssetBlobHeader {
  NYA_ConstCString path;
  u64              start;
  u64              size;
};

struct NYA_Asset {
  NYA_AssetStatus status;
  NYA_CString     path;
  u8*             data;
  u64             size;
  u64             path_modification_time;
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

NYA_API NYA_EXTERN void nya_system_asset_init(void);
NYA_API NYA_EXTERN void nya_system_asset_deinit(void);
NYA_API NYA_EXTERN void nya_system_asset_handle_event(NYA_Event* event);

/*
 * ─────────────────────────────────────────────────────────
 * ASSET FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

/// this is used in the build system
NYA_API NYA_EXTERN void nya_asset_generate_manifest(NYA_ConstCString asset_directory, NYA_ConstCString output_file);
NYA_API NYA_EXTERN void nya_asset_generate_embedding(NYA_ConstCString asset_directory, NYA_ConstCString output_file);

NYA_API NYA_EXTERN NYA_Asset* nya_asset_get(NYA_CString path);
NYA_API NYA_EXTERN NYA_Asset* nya_asset_load_and_get(NYA_CString path);
NYA_API NYA_EXTERN void       nya_asset_load_blocking(NYA_CString path);
NYA_API NYA_EXTERN void       nya_asset_load_async(NYA_CString path);
NYA_API NYA_EXTERN void       nya_asset_unload(NYA_Asset* asset);
