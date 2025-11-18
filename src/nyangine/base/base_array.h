#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_assert.h"
#include "nyangine/base/base_ints.h"
#include "nyangine/base/base_math.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_derive_array(type)                                                                                         \
  typedef struct {                                                                                                     \
    u64        length;                                                                                                 \
    u64        capacity;                                                                                               \
    type*      items;                                                                                                  \
    NYA_Arena* arena;                                                                                                  \
  } type##Array

nya_derive_array(b8);
nya_derive_array(b16);
nya_derive_array(b32);
nya_derive_array(b64);
nya_derive_array(u8);
nya_derive_array(u16);
nya_derive_array(u32);
nya_derive_array(u64);
nya_derive_array(s8);
nya_derive_array(s16);
nya_derive_array(s32);
nya_derive_array(s64);
nya_derive_array(f16);
nya_derive_array(f32);
nya_derive_array(f64);

#if defined(__has_feature) && __has_attribute(ext_vector_type)
nya_derive_array(f16x2);
nya_derive_array(f16x3);
nya_derive_array(f16x4);
nya_derive_array(f32x2);
nya_derive_array(f32x3);
nya_derive_array(f32x4);
nya_derive_array(f64x2);
nya_derive_array(f64x3);
nya_derive_array(f64x4);
#endif // defined(__has_feature) && __has_attribute(ext_vector_type)

#if defined(__has_feature) && __has_attribute(ext_vector_type) && __has_attribute(matrix_type)
nya_derive_array(f16_2x2);
nya_derive_array(f16_3x3);
nya_derive_array(f16_4x4);
nya_derive_array(f32_2x2);
nya_derive_array(f32_3x3);
nya_derive_array(f32_4x4);
nya_derive_array(f64_2x2);
nya_derive_array(f64_3x3);
nya_derive_array(f64_4x4);
#endif // defined(__has_feature) && __has_attribute(ext_vector_type) && __has_attribute(matrix_type)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * BASIC MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define _nya_array_access_guard(index, length)                                                                         \
  nya_assert(                                                                                                          \
      0 < (index) && (index) < (length),                                                                               \
      "Array index " FMTs64 " (length " FMTu64 ") out of bounds.",                                                     \
      nya_cast_to_s64(index),                                                                                          \
      length                                                                                                           \
  );

#define nya_array_new(arena_ptr, item_type) nya_array_new_with_capacity(arena_ptr, item_type, 16)
#define nya_array_new_with_capacity(arena_ptr, item_type, capacity)                                                    \
  ({                                                                                                                   \
    (item_type##Array){                                                                                                \
        .items    = nya_arena_alloc(arena_ptr, (capacity) * sizeof(item_type)),                                        \
        .length   = 0,                                                                                                 \
        .capacity = (capacity),                                                                                        \
        .arena    = (arena_ptr),                                                                                       \
    };                                                                                                                 \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ITERATOR MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_array_for(arr_ptr, index_name) for (u64 index_name = 0; (index_name) < (arr_ptr)->length; (index_name)++)

#define nya_array_for_reverse(arr_ptr, index_name)                                                                     \
  for (u64 index_name = (arr_ptr)->length - 1; (index_name) >= 0; (index_name)--)

#define nya_array_foreach(arr_ptr, item_name)                                                                          \
  for (typeof(*(arr_ptr)->items)*(item_name) = (arr_ptr)->items; (item_name) < (arr_ptr)->items + (arr_ptr)->length;   \
       (item_name)++)

#define nya_array_foreach_reverse(arr_ptr, item_name)                                                                  \
  for (typeof(*(arr_ptr)->items)*(item_name) = (arr_ptr)->items + (arr_ptr)->length - 1;                               \
       (item_name) >= (arr_ptr)->items;                                                                                \
       (item_name)--)
