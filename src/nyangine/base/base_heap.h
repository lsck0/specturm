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

#define nya_derive_heap(type)                                                                                                                        \
  typedef struct {                                                                                                                                   \
    u64        length;                                                                                                                               \
    u64        capacity;                                                                                                                             \
    type*      items;                                                                                                                                \
    NYA_Arena* arena;                                                                                                                                \
    s32 (*compare)(const type* a, const type* b);                                                                                                    \
  } type##Heap

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * CREATION MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define _NYA_HEAP_DEFAULT_CAPACITY 16

/**
 * Compare function: s32 compare_fn(const T* a, const T* b);
 * Return: -1 if a < b, 0 if a == b, 1 if a > b
 * */
#define nya_heap_create(arena_ptr, item_type, compare_fn) nya_heap_create_with_capacity(arena_ptr, item_type, compare_fn, _NYA_HEAP_DEFAULT_CAPACITY)
#define nya_heap_create_with_capacity(arena_ptr, item_type, compare_fn, initial_capacity)                                                            \
  ({                                                                                                                                                 \
    item_type##Heap* heap_ptr = nya_arena_alloc(arena_ptr, sizeof(item_type##Heap));                                                                 \
    *heap_ptr                 = (item_type##Heap){                                                                                                   \
                      .items    = nya_arena_alloc(arena_ptr, (initial_capacity) * sizeof(item_type)),                                                \
                      .length   = 0,                                                                                                                 \
                      .capacity = (initial_capacity),                                                                                                \
                      .arena    = (arena_ptr),                                                                                                       \
                      .compare  = (compare_fn),                                                                                                      \
    };                                                                                                                                               \
    heap_ptr;                                                                                                                                        \
  })

#define nya_heap_from_carray(arena_ptr, item_type, carray, carray_length, compare_fn)                                                                \
  ({                                                                                                                                                 \
    nya_assert_type_match(arena_ptr, (NYA_Arena*)0);                                                                                                 \
    nya_assert_type_match(carray, (item_type*)0);                                                                                                    \
    nya_assert_type_match(carray_length, (u64)0);                                                                                                    \
    item_type##Heap heap = nya_heap_create_with_capacity(arena_ptr, item_type, compare_fn, carray_length);                                           \
    for (u64 i = 0; i < carray_length; i++) { nya_heap_push(&heap, carray[i]); }                                                                     \
    heap;                                                                                                                                            \
  })

#define nya_heap_resize(heap_ptr, new_capacity)                                                                                                      \
  ({                                                                                                                                                 \
    (heap_ptr)->items = nya_arena_realloc(                                                                                                           \
        (heap_ptr)->arena,                                                                                                                           \
        (heap_ptr)->items,                                                                                                                           \
        (heap_ptr)->capacity * sizeof(*(heap_ptr)->items),                                                                                           \
        (new_capacity) * sizeof(*(heap_ptr)->items)                                                                                                  \
    );                                                                                                                                               \
    (heap_ptr)->capacity = new_capacity;                                                                                                             \
  })

#define nya_heap_reserve(heap_ptr, min_capacity)                                                                                                     \
  ({                                                                                                                                                 \
    if ((heap_ptr)->capacity < (min_capacity)) { /**/                                                                                                \
      nya_heap_resize((heap_ptr), nya_cast_to_u64(nya_max(2UL * (heap_ptr)->capacity, (min_capacity))));                                             \
    }                                                                                                                                                \
  })

#define nya_heap_clear(heap_ptr) ({ (heap_ptr)->length = 0; })

#define nya_heap_destroy(heap_ptr)                                                                                                                   \
  ({                                                                                                                                                 \
    nya_arena_free((heap_ptr)->arena, (heap_ptr)->items, sizeof(*(heap_ptr)->items) * (heap_ptr)->capacity);                                         \
    nya_arena_free((heap_ptr)->arena, heap_ptr, sizeof(*(heap_ptr)));                                                                                \
    (heap_ptr) = nullptr;                                                                                                                            \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ACCESS MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_heap_peek(heap_ptr)                                                                                                                      \
  ({                                                                                                                                                 \
    _nya_array_access_guard(0, heap_ptr->length);                                                                                                    \
    (heap_ptr)->items[0];                                                                                                                            \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ADD / INSERT / REMOVE MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_heap_push(heap_ptr, item)                                                                                                                \
  ({                                                                                                                                                 \
    nya_assert((heap_ptr)->compare != nullptr);                                                                                                      \
    nya_assert_type_match(item, (heap_ptr)->items[0]);                                                                                               \
    if ((heap_ptr)->length == (heap_ptr)->capacity) {                                                                                                \
      nya_heap_resize(heap_ptr, (heap_ptr)->capacity == 0 ? 1 : nya_cast_to_u64(2UL * (heap_ptr)->capacity));                                        \
    }                                                                                                                                                \
    u64 index                = (heap_ptr)->length++;                                                                                                 \
    (heap_ptr)->items[index] = item;                                                                                                                 \
    while (index != 0) {                                                                                                                             \
      u64 parent_index = (index - 1) / 2;                                                                                                            \
      if ((heap_ptr)->compare(&(heap_ptr)->items[index], &(heap_ptr)->items[parent_index]) < 0) {                                                    \
        nya_array_swap(heap_ptr, index, parent_index);                                                                                               \
        index = parent_index;                                                                                                                        \
      } else {                                                                                                                                       \
        break;                                                                                                                                       \
      }                                                                                                                                              \
    }                                                                                                                                                \
  })

#define nya_heap_pop(heap_ptr)                                                                                                                       \
  ({                                                                                                                                                 \
    nya_assert((heap_ptr)->compare != nullptr);                                                                                                      \
    nya_assert((heap_ptr)->length > 0);                                                                                                              \
    typeof(*(heap_ptr)->items) item = (heap_ptr)->items[0];                                                                                          \
    (heap_ptr)->items[0]            = (heap_ptr)->items[--(heap_ptr)->length];                                                                       \
    u64 index                       = 0;                                                                                                             \
    while (true) {                                                                                                                                   \
      u64 left_child_index     = 2 * index + 1;                                                                                                      \
      u64 right_child_index    = 2 * index + 2;                                                                                                      \
      u64 smallest_child_index = index;                                                                                                              \
      if (left_child_index < (heap_ptr)->length &&                                                                                                   \
          (heap_ptr)->compare(&(heap_ptr)->items[left_child_index], &(heap_ptr)->items[smallest_child_index]) < 0) {                                 \
        smallest_child_index = left_child_index;                                                                                                     \
      }                                                                                                                                              \
      if (right_child_index < (heap_ptr)->length &&                                                                                                  \
          (heap_ptr)->compare(&(heap_ptr)->items[right_child_index], &(heap_ptr)->items[smallest_child_index]) < 0) {                                \
        smallest_child_index = right_child_index;                                                                                                    \
      }                                                                                                                                              \
      if (smallest_child_index != index) {                                                                                                           \
        nya_array_swap(heap_ptr, index, smallest_child_index);                                                                                       \
        index = smallest_child_index;                                                                                                                \
      } else {                                                                                                                                       \
        break;                                                                                                                                       \
      }                                                                                                                                              \
    }                                                                                                                                                \
    item;                                                                                                                                            \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * MISC MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_heap_length(heap_ptr) ((heap_ptr)->length)
