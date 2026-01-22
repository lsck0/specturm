#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_string.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCT
 * ─────────────────────────────────────────────────────────
 */

typedef struct NYA_AssetSystem NYA_AssetSystem;

struct NYA_AssetSystem {
  NYA_Arena allocator;
};

/*
 * ─────────────────────────────────────────────────────────
 * ASSET STRUCTS
 * ─────────────────────────────────────────────────────────
 */

typedef struct {
  NYA_ConstCString path;
  u64              start;
  u64              size;
} NYA_AssetHeader;

NYA_API NYA_EXTERN const u64             NYA_ASSET_BLOB_HEADER_COUNT;
NYA_API NYA_EXTERN const NYA_AssetHeader NYA_ASSET_BLOB_HEADER[];
NYA_API NYA_EXTERN const u8              NYA_ASSET_BLOB[];

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

NYA_API NYA_EXTERN void nya_asset_generate_embedding(NYA_ConstCString asset_directory, NYA_ConstCString output_file);
