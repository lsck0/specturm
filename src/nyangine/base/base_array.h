#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_assert.h"
#include "nyangine/base/base_memory.h"
#include "nyangine/base/base_types.h"
#include "nyangine/math/math.h"

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
nya_derive_array(b128);
nya_derive_array(u8);
nya_derive_array(u16);
nya_derive_array(u32);
nya_derive_array(u64);
nya_derive_array(u128);
nya_derive_array(s8);
nya_derive_array(s16);
nya_derive_array(s32);
nya_derive_array(s64);
nya_derive_array(s128);
nya_derive_array(f16);
nya_derive_array(f32);
nya_derive_array(f64);
nya_derive_array(f128);

nya_derive_array(voidptr);
nya_derive_array(b8ptr);
nya_derive_array(b16ptr);
nya_derive_array(b32ptr);
nya_derive_array(b64ptr);
nya_derive_array(b128ptr);
nya_derive_array(u0ptr);
nya_derive_array(u8ptr);
nya_derive_array(u16ptr);
nya_derive_array(u32ptr);
nya_derive_array(u64ptr);
nya_derive_array(u128ptr);
nya_derive_array(s8ptr);
nya_derive_array(s16ptr);
nya_derive_array(s32ptr);
nya_derive_array(s64ptr);
nya_derive_array(s128ptr);
nya_derive_array(f16ptr);
nya_derive_array(f32ptr);
nya_derive_array(f64ptr);
nya_derive_array(f128ptr);

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
nya_derive_array(f128x2);
nya_derive_array(f128x3);
nya_derive_array(f128x4);
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
nya_derive_array(f128_2x2);
nya_derive_array(f128_3x3);
nya_derive_array(f128_4x4);
#endif // defined(__has_feature) && __has_attribute(ext_vector_type) && __has_attribute(matrix_type)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * CREATION MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define _NYA_ARRAY_DEFAULT_CAPACITY 16

#define nya_array_new(arena_ptr, item_type)                                                                            \
  nya_array_new_with_capacity(arena_ptr, item_type, _NYA_ARRAY_DEFAULT_CAPACITY)
#define nya_array_new_with_capacity(arena_ptr, item_type, initial_capacity)                                            \
  (item_type##Array) {                                                                                                 \
    .items = nya_arena_alloc(arena_ptr, (initial_capacity) * sizeof(item_type)), .length = 0,                          \
    .capacity = (initial_capacity), .arena = (arena_ptr),                                                              \
  }

#define nya_array_from_carray(arena_ptr, item_type, carray, carray_length)                                             \
  ({                                                                                                                   \
    nya_assert_type_match(arena_ptr, (NYA_Arena*)0);                                                                   \
    nya_assert_type_match(carray, (item_type*)0);                                                                      \
    nya_assert_type_match(carray_length, (u64)0);                                                                      \
    item_type##Array arr = nya_array_new_with_capacity(arena_ptr, item_type, carray_length);                           \
    nya_range_for_t (u64, idx, 0, carray_length) nya_array_push_back(&arr, (carray)[idx]);                             \
    arr;                                                                                                               \
  })

#define nya_array_from_argv(arena_ptr, argc, argv)                                                                     \
  ({                                                                                                                   \
    nya_assert_type_match(arena_ptr, (NYA_Arena*)0);                                                                   \
    nya_assert_type_match(argv, (const char**)0);                                                                      \
    NYA_StringArray args = nya_array_new_with_capacity(&arena, string, argc);                                          \
    nya_range_for (idx, 0, argc) nya_array_push_back(&args, nya_string_from(&arena, (argv)[idx]));                     \
    args;                                                                                                              \
  })

#define nya_array_resize(arr_ptr, new_capacity)                                                                        \
  ({                                                                                                                   \
    (arr_ptr)->items = nya_arena_realloc(                                                                              \
        (arr_ptr)->arena,                                                                                              \
        (arr_ptr)->items,                                                                                              \
        (arr_ptr)->capacity * sizeof(*(arr_ptr)->items),                                                               \
        (new_capacity) * sizeof(*(arr_ptr)->items)                                                                     \
    );                                                                                                                 \
    (arr_ptr)->capacity = new_capacity;                                                                                \
  })

#define nya_array_reserve(arr_ptr, min_capacity)                                                                       \
  ({                                                                                                                   \
    if ((arr_ptr)->capacity < (min_capacity)) {                                                                        \
      nya_array_resize((arr_ptr), nya_cast_to_u64(nya_max(2UL * (arr_ptr)->capacity, (min_capacity))));                \
    }                                                                                                                  \
  })

#define nya_array_clear(arr_ptr) ({ (arr_ptr)->length = 0; })

#define nya_array_destroy(arr_ptr)                                                                                     \
  ({                                                                                                                   \
    nya_arena_free((arr_ptr)->arena, (arr_ptr)->items, sizeof(*(arr_ptr)->items) * (arr_ptr)->capacity);               \
    (arr_ptr)->items    = nullptr;                                                                                     \
    (arr_ptr)->length   = 0;                                                                                           \
    (arr_ptr)->capacity = 0;                                                                                           \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ACCESS MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define _nya_array_access_guard(index, length)                                                                         \
  nya_assert(                                                                                                          \
      0 <= (index) && (index) < (length),                                                                              \
      "Array index " FMTs64 " (length " FMTu64 ") out of bounds.",                                                     \
      (s64)(index),                                                                                                    \
      length                                                                                                           \
  );

#define nya_array_get(arr_ptr, index)                                                                                  \
  ({                                                                                                                   \
    _nya_array_access_guard(index, (arr_ptr)->length);                                                                 \
    (arr_ptr)->items[index];                                                                                           \
  })

#define nya_array_set(arr_ptr, index, item)                                                                            \
  ({                                                                                                                   \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    _nya_array_access_guard(index, (arr_ptr)->length);                                                                 \
    (arr_ptr)->items[index] = item;                                                                                    \
  })

#define nya_array_first(arr_ptr) nya_array_get(arr_ptr, 0)
#define nya_array_last(arr_ptr)  nya_array_get(arr_ptr, (arr_ptr)->length - 1)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ADD / INSERT / REMOVE MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_array_add(arr_ptr, item)                                                                                   \
  ({                                                                                                                   \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    if ((arr_ptr)->length == (arr_ptr)->capacity) {                                                                    \
      nya_array_resize(arr_ptr, nya_cast_to_u64(2UL * (arr_ptr)->capacity));                                           \
    }                                                                                                                  \
    (arr_ptr)->items[(arr_ptr)->length++] = item;                                                                      \
  })

#define nya_array_add_many(arr_ptr, ...)                                                                               \
  ({                                                                                                                   \
    typeof(*(arr_ptr)->items) items_to_add[]     = {__VA_ARGS__};                                                      \
    u64                       items_to_add_count = sizeof(items_to_add) / sizeof(items_to_add[0]);                     \
    nya_array_reserve(arr_ptr, (arr_ptr)->length + items_to_add_count);                                                \
    for (u64 i = 0; i < items_to_add_count; i++) {                                                                     \
      nya_assert_type_match(items_to_add[i], (arr_ptr)->items[0]);                                                     \
      (arr_ptr)->items[(arr_ptr)->length++] = items_to_add[i];                                                         \
    }                                                                                                                  \
  })

#define nya_array_extend(arr_ptr, other_arr_ptr)                                                                       \
  ({                                                                                                                   \
    nya_assert_type_match((arr_ptr)->items[0], (other_arr_ptr)->items[0]);                                             \
    nya_array_reserve(arr_ptr, (arr_ptr)->length + (other_arr_ptr)->length);                                           \
    nya_memmove(                                                                                                       \
        (arr_ptr)->items + (arr_ptr)->length,                                                                          \
        (other_arr_ptr)->items,                                                                                        \
        (other_arr_ptr)->length * sizeof(*(other_arr_ptr)->items)                                                      \
    );                                                                                                                 \
    (arr_ptr)->length += (other_arr_ptr)->length;                                                                      \
  })

#define nya_array_insert(arr_ptr, item, index)                                                                         \
  ({                                                                                                                   \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    _nya_array_access_guard(index, (arr_ptr)->length);                                                                 \
    if ((arr_ptr)->length == (arr_ptr)->capacity) {                                                                    \
      nya_array_resize(arr_ptr, nya_cast_to_u64(2UL * (arr_ptr)->capacity));                                           \
    }                                                                                                                  \
    nya_memmove(                                                                                                       \
        (arr_ptr)->items + (index) + 1,                                                                                \
        (arr_ptr)->items + (index),                                                                                    \
        ((arr_ptr)->length * sizeof(*(arr_ptr)->items) - (index))                                                      \
    );                                                                                                                 \
    (arr_ptr)->items[index] = (item);                                                                                  \
    (arr_ptr)->length++;                                                                                               \
  })

#define nya_array_insert_many(arr_ptr, start_index, ...)                                                               \
  ({                                                                                                                   \
    typeof(*(arr_ptr)->items) items_to_add[]     = {__VA_ARGS__};                                                      \
    u64                       items_to_add_count = sizeof(items_to_add) / sizeof(items_to_add[0]);                     \
    _nya_array_access_guard(start_index, (arr_ptr)->length);                                                           \
    nya_array_reserve(arr_ptr, (arr_ptr)->length + items_to_add_count);                                                \
    nya_memmove(                                                                                                       \
        (arr_ptr)->items + (start_index) + items_to_add_count,                                                         \
        (arr_ptr)->items + (start_index),                                                                              \
        ((arr_ptr)->length * sizeof(*(arr_ptr)->items) - (start_index))                                                \
    );                                                                                                                 \
    for (u64 i = 0; i < items_to_add_count; i++) {                                                                     \
      nya_assert_type_match(items_to_add[i], (arr_ptr)->items[0]);                                                     \
      (arr_ptr)->items[start_index + i] = items_to_add[i];                                                             \
    }                                                                                                                  \
    (arr_ptr)->length += items_to_add_count;                                                                           \
  })

#define nya_array_remove(arr_ptr, index)                                                                               \
  ({                                                                                                                   \
    _nya_array_access_guard(index, (arr_ptr)->length);                                                                 \
    typeof(*(arr_ptr)->items) item = (arr_ptr)->items[index];                                                          \
    if ((index) != (arr_ptr)->length - 1) {                                                                            \
      nya_memmove(                                                                                                     \
          (arr_ptr)->items + (index),                                                                                  \
          (arr_ptr)->items + (index) + 1,                                                                              \
          ((arr_ptr)->length - (index) - 1) * sizeof(*(arr_ptr)->items)                                                \
      );                                                                                                               \
    }                                                                                                                  \
    (arr_ptr)->length--;                                                                                               \
    item;                                                                                                              \
  })

#define nya_array_remove_many(arr_ptr, start_index, count)                                                             \
  ({                                                                                                                   \
    _nya_array_access_guard(start_index, (arr_ptr)->length);                                                           \
    nya_assert((start_index) + (count) <= (arr_ptr)->length);                                                          \
    if ((start_index) + (count) != (arr_ptr)->length) {                                                                \
      nya_memmove(                                                                                                     \
          (arr_ptr)->items + (start_index),                                                                            \
          (arr_ptr)->items + (start_index) + (count),                                                                  \
          ((arr_ptr)->length * sizeof(*(arr_ptr)->items) - (start_index) - (count))                                    \
      );                                                                                                               \
    }                                                                                                                  \
    (arr_ptr)->length -= (count);                                                                                      \
  })

#define nya_array_remove_item(arr_ptr, item)                                                                           \
  ({                                                                                                                   \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    typeof((arr_ptr)->items[0]) item_var = item;                                                                       \
    for (u64 i = 0; i < (arr_ptr)->length; i++) {                                                                      \
      if (nya_memcmp(&(arr_ptr)->items[i], &item_var, sizeof(item_var)) == 0) {                                        \
        nya_array_remove(arr_ptr, i);                                                                                  \
        break;                                                                                                         \
      }                                                                                                                \
    }                                                                                                                  \
  })

#define nya_array_push_back(arr_ptr, item)       nya_array_add(arr_ptr, item)
#define nya_array_push_back_many(arr_ptr, ...)   nya_array_add_many(arr_ptr, __VA_ARGS__)
#define nya_array_push_front(arr_ptr, item)      nya_array_insert(arr_ptr, item, 0)
#define nya_array_push_front_many(arr_ptr, ...)  nya_array_insert_many(arr_ptr, 0, __VA_ARGS__)
#define nya_array_pop_back(arr_ptr)              nya_array_remove(arr_ptr, (arr_ptr)->length - 1)
#define nya_array_pop_back_many(arr_ptr, count)  nya_array_remove_many(arr_ptr, (arr_ptr)->length - (count), count)
#define nya_array_pop_front(arr_ptr)             nya_array_remove(arr_ptr, 0)
#define nya_array_pop_front_many(arr_ptr, count) nya_array_remove_many(arr_ptr, 0, count)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FIND / SORT MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_array_contains(arr_ptr, item)                                                                              \
  ({                                                                                                                   \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    typeof((arr_ptr)->items[0]) item_var = item;                                                                       \
    b8                          contains = false;                                                                      \
    nya_array_for (arr_ptr, arr_index) {                                                                               \
      if (nya_memcmp(&(arr_ptr)->items[arr_index], &item_var, sizeof(item_var)) == 0) {                                \
        contains = true;                                                                                               \
        break;                                                                                                         \
      }                                                                                                                \
    }                                                                                                                  \
    contains;                                                                                                          \
  })

#define nya_array_find(arr_ptr, item)                                                                                  \
  ({                                                                                                                   \
    nya_assert_type_match(item, (arr_ptr)->items[0]);                                                                  \
    typeof((arr_ptr)->items[0]) item_var = item;                                                                       \
    s64                         index    = 0;                                                                          \
    nya_array_for (arr_ptr, item_index) {                                                                              \
      if (nya_memcmp(&(arr_ptr)->items[item_index], &item_var, sizeof(item_var)) == 0) { break; }                      \
      index++;                                                                                                         \
    }                                                                                                                  \
    (u64) index == (arr_ptr)->length ? -1 : index;                                                                     \
  })

/**
 * Compare function: s32 compare_fn(const T* a, const T* b);
 * Return: -1 if a < b, 0 if a == b, 1 if a > b
 * */
#define nya_array_sort(arr_ptr, compare_fn)                                                                            \
  ({                                                                                                                   \
    nya_assert_type_match(                                                                                             \
        &(compare_fn),                                                                                                 \
        (s32 (*)(const typeof((arr_ptr)->items[0])*, const typeof((arr_ptr)->items[0])*))0                             \
    );                                                                                                                 \
    qsort(                                                                                                             \
        (arr_ptr)->items,                                                                                              \
        (arr_ptr)->length,                                                                                             \
        sizeof(*(arr_ptr)->items),                                                                                     \
        (int (*)(const void*, const void*))(compare_fn)                                                                \
    );                                                                                                                 \
  })

#define nya_array_equals(arr1_ptr, arr2_ptr)                                                                           \
  ({                                                                                                                   \
    nya_assert_type_match((arr1_ptr)->items[0], (arr2_ptr)->items[0]);                                                 \
    b8 equal = (arr1_ptr)->length == (arr2_ptr)->length;                                                               \
    if (equal) {                                                                                                       \
      for (u64 i = 0; i < (arr1_ptr)->length; i++) {                                                                   \
        if (nya_memcmp(&(arr1_ptr)->items[i], &(arr2_ptr)->items[i], sizeof((arr1_ptr)->items[i])) != 0) {             \
          equal = false;                                                                                               \
          break;                                                                                                       \
        }                                                                                                              \
      }                                                                                                                \
    }                                                                                                                  \
    equal;                                                                                                             \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * MISC MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_carray_length(carray) (sizeof(carray) / sizeof((carray)[0]))
#define nya_array_length(arr_ptr) ((arr_ptr)->length)

#define nya_array_swap(arr_ptr, index1, index2)                                                                        \
  ({                                                                                                                   \
    _nya_array_access_guard(index1, (arr_ptr)->length);                                                                \
    _nya_array_access_guard(index2, (arr_ptr)->length);                                                                \
    typeof(*(arr_ptr)->items) tmp = (arr_ptr)->items[index1];                                                          \
    (arr_ptr)->items[index1]      = (arr_ptr)->items[index2];                                                          \
    (arr_ptr)->items[index2]      = tmp;                                                                               \
  })

#define nya_array_reverse(arr_ptr)                                                                                     \
  ({                                                                                                                   \
    for (u64 i = 0; i < (arr_ptr)->length / 2; i++) nya_array_swap(arr_ptr, i, (arr_ptr)->length - i - 1);             \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * MEMORY MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_array_clone(arr_ptr)                                                                                         \
  ({                                                                                                                     \
    typeof(*(arr_ptr)) copy = {                                                                                          \
        .items    = nya_arena_copy((arr_ptr)->arena, (arr_ptr)->items, sizeof(*(arr_ptr)->items) * (arr_ptr)->capacity), \
        .length   = (arr_ptr)->length,                                                                                   \
        .capacity = (arr_ptr)->capacity,                                                                                 \
        .arena    = (arr_ptr)->arena                                                                                     \
    };                                                                                                                   \
    copy;                                                                                                                \
  })

#define nya_array_move(arr_ptr, new_arena_ptr)                                                                         \
  ({                                                                                                                   \
    nya_assert_type_match(new_arena_ptr, (arr_ptr)->arena);                                                            \
    *(arr_ptr) = (typeof(*(arr_ptr))){.items = nya_arena_move(                                                         \
                                          (arr_ptr)->arena,                                                            \
                                          new_arena_ptr,                                                               \
                                          (arr_ptr)->items,                                                            \
                                          sizeof(*(arr_ptr)->items) * (arr_ptr)->capacity                              \
                                      ),                                                                               \
                                      .length   = (arr_ptr)->length,                                                   \
                                      .capacity = (arr_ptr)->capacity,                                                 \
                                      .arena    = (new_arena_ptr)};                                                       \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * SLICE MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_array_slice_excld(arr_ptr, start, end)                                                                     \
  ({                                                                                                                   \
    _nya_array_access_guard(start, (arr_ptr)->length);                                                                 \
    _nya_array_access_guard((end) - 1, (arr_ptr)->length);                                                             \
    typeof(*(arr_ptr)) slice = {                                                                                       \
        .items    = &(arr_ptr)->items[start],                                                                          \
        .length   = (end) - (start),                                                                                   \
        .capacity = (end) - (start),                                                                                   \
        .arena    = nullptr,                                                                                           \
    };                                                                                                                 \
    slice;                                                                                                             \
  })

#define nya_array_slice_incld(arr_ptr, start, end)                                                                     \
  ({                                                                                                                   \
    _nya_array_access_guard(start, (arr_ptr)->length);                                                                 \
    _nya_array_access_guard(end, (arr_ptr)->length);                                                                   \
    typeof(*(arr_ptr)) slice = {                                                                                       \
        .items    = &(arr_ptr)->items[start],                                                                          \
        .length   = (end) - (start) + 1,                                                                               \
        .capacity = (end) - (start) + 1,                                                                               \
        .arena    = nullptr                                                                                            \
    };                                                                                                                 \
    slice;                                                                                                             \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ITERATOR MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_range_for_t(type, idx, start, end) for (type idx = start; (idx) < (end); (idx)++)
#define nya_range_for(idx, start, end)         nya_range_for_t (s32, idx, start, end)

#define nya_array_for(arr_ptr, index_name) for (u64 index_name = 0; (index_name) < (arr_ptr)->length; (index_name)++)

#define nya_array_for_reverse(arr_ptr, index_name)                                                                     \
  for (u64 index_name = (arr_ptr)->length; (index_name) > 0 && ((index_name)-- || 1);)

#define nya_array_foreach(arr_ptr, item_name)                                                                          \
  for (typeof(*(arr_ptr)->items)*(item_name) = (arr_ptr)->items; (item_name) < (arr_ptr)->items + (arr_ptr)->length;   \
       (item_name)++)

#define nya_array_foreach_reverse(arr_ptr, item_name)                                                                  \
  for (typeof(*(arr_ptr)->items)*(item_name) = (arr_ptr)->items + (arr_ptr)->length;                                   \
       (item_name) > (arr_ptr)->items && ((item_name)-- || 1);)
