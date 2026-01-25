#pragma once

#include "SDL3/SDL_gpu.h"

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

typedef NYA_CString                NYA_AssetHandle;
typedef struct NYA_Asset           NYA_Asset;
typedef struct NYA_AssetBlobHeader NYA_AssetBlobHeader;
typedef struct NYA_AssetLoadParams NYA_AssetLoadParams;
typedef struct NYA_AssetSystem     NYA_AssetSystem;
nya_derive_array(NYA_AssetLoadParams);
nya_derive_hmap(NYA_AssetHandle, NYA_Asset);

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM TYPES
 * ─────────────────────────────────────────────────────────
 */

struct NYA_AssetSystem {
  NYA_Arena allocator;

  NYA_AssetHandle_NYA_Asset_HMap assets;
  NYA_CStringArray               changed_assets;

  b8                       batch_active;
  u32Array                 batch_types;
  NYA_AssetLoadParamsArray batch_requests;
};

/*
 * ─────────────────────────────────────────────────────────
 * ASSET TYPES
 * ─────────────────────────────────────────────────────────
 */

struct NYA_AssetBlobHeader {
  NYA_ConstCString handle;
  u64              start;
  u64              size;
};

typedef enum {
  NYA_ASSET_TYPE_UNKNOWN,
  NYA_ASSET_TYPE_TEXT,
  NYA_ASSET_TYPE_TEXTURE,
  NYA_ASSET_TYPE_SHADER_VERTEX,
  NYA_ASSET_TYPE_SHADER_FRAGMENT,
  NYA_ASSET_TYPE_SHADER_COMPUTE,
  NYA_ASSET_TYPE_BUFFER_VERTEX,
  NYA_ASSET_TYPE_BUFFER_INDEX,
  NYA_ASSET_TYPE_BUFFER_UNIFORM,
  NYA_ASSET_TYPE_GRAPHICS_PIPELINE,
  NYA_ASSET_TYPE_COUNT,
} NYA_AssetType;

struct NYA_AssetLoadParams {
  NYA_AssetHandle handle;

  union {
    struct {
      u32                     size;
      SDL_GPUBufferUsageFlags usage;
    } buffer;
    struct {
      u32 num_samplers;
      u32 num_storage_textures;
      u32 num_storage_buffers;
      u32 num_uniform_buffers;
    } shader;
    struct {
      NYA_CString                       vertex_shader_path;
      NYA_CString                       fragment_shader_path;
      SDL_GPUGraphicsPipelineCreateInfo create_info;
    } pipeline;
  };
};

typedef enum {
  NYA_ASSET_STATUS_UNLOADED,
  NYA_ASSET_STATUS_LOADED,
  NYA_ASSET_STATUS_COUNT,
} NYA_AssetStatus;

struct NYA_Asset {
  NYA_AssetType   type;
  NYA_AssetStatus status;
  NYA_AssetHandle handle;
  u8*             data;
  u64             size;
  u64             source_timestamp;

  union {
    SDL_GPUTexture*          texture;
    SDL_GPUShader*           shader;
    SDL_GPUBuffer*           buffer;
    SDL_GPUGraphicsPipeline* graphics_pipeline;
  } gpu_resource;

  atomic u32 reference_count;
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

// this is used in the build system
NYA_API NYA_EXTERN void nya_asset_generate_manifest(NYA_ConstCString asset_directory, NYA_ConstCString output_file);
NYA_API NYA_EXTERN void nya_asset_generate_embedding(NYA_ConstCString asset_directory, NYA_ConstCString output_file);

NYA_API NYA_EXTERN void       nya_asset_load(NYA_AssetType type, NYA_AssetHandle handle, NYA_AssetLoadParams params);
NYA_API NYA_EXTERN void       nya_asset_unload(NYA_AssetHandle handle);
NYA_API NYA_EXTERN NYA_Asset* nya_asset_acquire(NYA_AssetHandle handle);
NYA_API NYA_EXTERN void       nya_asset_release(NYA_AssetHandle handle);
NYA_API NYA_EXTERN NYA_Asset* nya_asset_load_and_acquire(NYA_AssetType type, NYA_AssetHandle handle, NYA_AssetLoadParams params);

NYA_API NYA_EXTERN void nya_asset_load_batch_begin(void);
NYA_API NYA_EXTERN void nya_asset_load_batch_add(NYA_AssetType type, NYA_AssetLoadParams params);
NYA_API NYA_EXTERN void nya_asset_load_batch_submit(void);
