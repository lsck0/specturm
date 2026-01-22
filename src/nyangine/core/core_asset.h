#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_string.h"

#include "../../../assets/assets.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_Asset           NYA_Asset;
typedef struct NYA_AssetBlobHeader NYA_AssetBlobHeader;
typedef struct NYA_AssetHandle     NYA_AssetHandle;
typedef struct NYA_AssetSystem     NYA_AssetSystem;

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_AssetSystem {
  NYA_Arena allocator;
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
} NYA_AssetStatus;

struct NYA_AssetBlobHeader {
  NYA_ConstCString path;
  u64              start;
  u64              size;
};

struct NYA_Asset {
  NYA_AssetStatus  status;
  NYA_ConstCString path;
  u8*              data;
  u64              size;
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

/*
 * ─────────────────────────────────────────────────────────
 * ASSET FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

/// this is used in the build system
NYA_API NYA_EXTERN void nya_asset_generate_manifest(NYA_ConstCString asset_directory, NYA_ConstCString output_file);
NYA_API NYA_EXTERN void nya_asset_generate_embedding(NYA_ConstCString asset_directory, NYA_ConstCString output_file);

NYA_API NYA_EXTERN NYA_Asset* nya_asset_get(NYA_ConstCString path);
NYA_API NYA_EXTERN NYA_Asset* nya_asset_loaded_and_get(NYA_ConstCString path);
NYA_API NYA_EXTERN void       nya_asset_load_blocking(NYA_ConstCString path);
NYA_API NYA_EXTERN void       nya_asset_load_async(NYA_ConstCString path);
NYA_API NYA_EXTERN b8         nya_asset_is_loaded(NYA_ConstCString path);
NYA_API NYA_EXTERN void       nya_asset_unload(NYA_ConstCString path);
