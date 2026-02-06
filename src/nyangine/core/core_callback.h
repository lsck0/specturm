#pragma once

#include "nyangine/base/base_array.h"
#include "nyangine/base/base_string.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_Callback       NYA_Callback;
typedef struct NYA_CallbackSystem NYA_CallbackSystem;
typedef u64                       NYA_CallbackHandle;
nya_derive_array(NYA_Callback);

/*
 * ─────────────────────────────────────────────────────────
 * SYSTEM STRUCTS
 * ─────────────────────────────────────────────────────────
 */

struct NYA_CallbackSystem {
  NYA_Arena*         allocator;
  NYA_CallbackArray* callbacks;
};

/*
 * ─────────────────────────────────────────────────────────
 * CALLBACK STRUCTS
 * ─────────────────────────────────────────────────────────
 */

struct NYA_Callback {
  NYA_ConstCString name;
  void*            fn;
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

NYA_API NYA_EXTERN void nya_system_callback_init(void);
NYA_API NYA_EXTERN void nya_system_callback_deinit(void);

/*
 * ─────────────────────────────────────────────────────────
 * CALLBACK FUNCTIONS
 * ─────────────────────────────────────────────────────────
 */

#define nya_callback(callback) _nya_callback((NYA_Callback){ .name = #callback, .fn = (void*)(callback) })
NYA_API NYA_EXTERN void* nya_callback_get(NYA_CallbackHandle handle);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * INTERNAL
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN NYA_CallbackHandle _nya_callback(NYA_Callback callback);
