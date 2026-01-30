#pragma once

#include "SDL3/SDL_gpu.h"

#include "nyangine/base/base.h"
#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
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

typedef NYA_CString                    NYA_AssetHandle;
typedef enum NYA_AssetLoadStatus       NYA_AssetStatus;
typedef enum NYA_AssetType             NYA_AssetType;
typedef struct NYA_Asset               NYA_Asset;
typedef struct NYA_AssetBlobHeader     NYA_AssetBlobHeader;
typedef struct NYA_AssetLoadParameters NYA_AssetLoadParameters;
typedef struct NYA_AssetSystem         NYA_AssetSystem;
nya_derive_array(NYA_AssetHandle);
nya_derive_array(NYA_AssetLoadParameters);
nya_derive_hmap(NYA_AssetHandle, NYA_Asset);

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCT
 * ─────────────────────────────────────────────────────────
 */

struct NYA_AssetSystem {
  NYA_Arena* allocator;

  NYA_AssetHandle_NYA_Asset_HMap* assets;
  NYA_AssetLoadParametersArray*   loading_queue;
  NYA_AssetHandleArray*           unloading_queue;

#ifdef NYA_ASSET_BACKEND_FS
  NYA_AssetHandleArray* reload_queue;
#endif // NYA_ASSET_BACKEND_FS
};

/*
 * ─────────────────────────────────────────────────────────
 * ASSET STRUCTS
 * ─────────────────────────────────────────────────────────
 */

struct NYA_AssetBlobHeader {
  NYA_ConstCString path;
  u64              start;
  u64              size;
};

enum NYA_AssetType {
  // raw data
  NYA_ASSET_TYPE_TEXT,

  // processed data on cpu ram
  NYA_ASSET_TYPE_SOUND,

  // processed data on gpu vram
  NYA_ASSET_TYPE_TEXTURE,
  NYA_ASSET_TYPE_SHADER_VERTEX,
  NYA_ASSET_TYPE_SHADER_FRAGMENT,
  NYA_ASSET_TYPE_SHADER_COMPUTE,
  NYA_ASSET_TYPE_BUFFER_VERTEX,
  NYA_ASSET_TYPE_BUFFER_INDEX,
  NYA_ASSET_TYPE_BUFFER_UNIFORM,

  // meta assets ? things that are made up of other assets
  NYA_ASSET_TYPE_GRAPHICS_PIPELINE,

  NYA_ASSET_TYPE_COUNT,
};

enum NYA_AssetLoadStatus {
  NYA_ASSET_STATUS_UNLOADED,
  NYA_ASSET_STATUS_LOADING,
  NYA_ASSET_STATUS_LOADED,
  NYA_ASSET_STATUS_COUNT,
};

struct NYA_AssetLoadParameters {
  NYA_AssetType   type;
  NYA_AssetHandle handle;

  union {
    struct {
      u32 num_samplers;
      u32 num_storage_textures;
      u32 num_storage_buffers;
      u32 num_uniform_buffers;
    } as_shader;
  };
};

struct NYA_Asset {
  NYA_AssetType           type;
  NYA_AssetHandle         handle;
  NYA_AssetStatus         status;
  NYA_AssetLoadParameters load_parameters;

  union {
    struct {
      u8* data;
      u64 size;
    } as_text;

    struct {
      NYA_AssetHandle     compiled_handle;
      SDL_GPUShaderFormat format;
      SDL_GPUShader*      shader;
    } as_shader;
  };

  atomic u64 reference_count;

#ifdef NYA_ASSET_BACKEND_FS
  u64 source_modification_time;
#endif // NYA_ASSET_BACKEND_FS
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

#define nya_asset_with(asset) if ((asset) && (asset)->status == NYA_ASSET_STATUS_LOADED)

/// this is used in the build system
NYA_API NYA_EXTERN void nya_asset_generate_manifest(NYA_ConstCString asset_directory, NYA_ConstCString output_file);
NYA_API NYA_EXTERN void nya_asset_generate_embedding(NYA_ConstCString asset_directory, NYA_ConstCString output_file);

NYA_API NYA_EXTERN NYA_Asset* nya_asset_get(NYA_AssetHandle handle);
NYA_API NYA_EXTERN void       nya_asset_acquire(NYA_AssetHandle handle);
NYA_API NYA_EXTERN void       nya_asset_release(NYA_AssetHandle handle);
NYA_API NYA_EXTERN void       nya_asset_load(NYA_AssetLoadParameters parameters);
NYA_API NYA_EXTERN void       nya_asset_unload(NYA_Asset* asset);
