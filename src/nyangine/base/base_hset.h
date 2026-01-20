#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_string.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define _NYA_HASHSET_DEFAULT_CAPACITY 64
#define _NYA_HASHSET_LOAD_FACTOR      0.75F

#define nya_derive_hset(item_type)                                                                                     \
  typedef struct {                                                                                                     \
    u64        length;                                                                                                 \
    u64        capacity;                                                                                               \
    item_type* items;                                                                                                  \
    b8*        occupied;                                                                                               \
    NYA_Arena* arena;                                                                                                  \
  } item_type##HSet

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * CREATION MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_hset_create(arena_ptr, item_type)                                                                          \
  nya_hset_create_with_capacity(arena_ptr, item_type, _NYA_HASHSET_DEFAULT_CAPACITY)
#define nya_hset_create_with_capacity(arena_ptr, item_type, initial_capacity)                                          \
  ({                                                                                                                   \
    item_type##HSet _hset = {                                                                                          \
        .length   = 0,                                                                                                 \
        .capacity = (initial_capacity),                                                                                \
        .arena    = (arena_ptr),                                                                                       \
        .items    = nya_arena_alloc(arena_ptr, (initial_capacity) * sizeof(item_type)),                                \
        .occupied = nya_arena_alloc(arena_ptr, (initial_capacity) * sizeof(b8)),                                       \
    };                                                                                                                 \
    nya_memset(_hset.occupied, 0, (initial_capacity) * sizeof(b8));                                                    \
    _hset;                                                                                                             \
  })

#define nya_hset_clear(hset_ptr)                                                                                       \
  ({                                                                                                                   \
    nya_memset((hset_ptr)->occupied, 0, (hset_ptr)->capacity * sizeof(b8));                                            \
    (hset_ptr)->length = 0;                                                                                            \
  })

#define nya_hset_destroy(hset_ptr)                                                                                     \
  ({                                                                                                                   \
    nya_arena_free((hset_ptr)->arena, (hset_ptr)->items, sizeof(*(hset_ptr)->items) * (hset_ptr)->capacity);           \
    nya_arena_free((hset_ptr)->arena, (hset_ptr)->occupied, sizeof(*(hset_ptr)->occupied) * (hset_ptr)->capacity);     \
    (hset_ptr)->items    = nullptr;                                                                                    \
    (hset_ptr)->occupied = nullptr;                                                                                    \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * RESIZE AND REHASH MACRO
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_hset_insert_unchecked(hset_ptr, item)                                                                      \
  ({                                                                                                                   \
    nya_assert_type_match(item, (hset_ptr)->items[0]);                                                                 \
    typeof(item) item_var   = item;                                                                                    \
    u64          index      = nya_hash_fnv1a(&item_var, sizeof(item_var)) % (hset_ptr)->capacity;                      \
    u64          iterations = 0;                                                                                       \
    b8           found      = false;                                                                                   \
    while (iterations < (hset_ptr)->capacity) {                                                                        \
      if (!(hset_ptr)->occupied[index]) {                                                                              \
        (hset_ptr)->items[index]    = item_var;                                                                        \
        (hset_ptr)->occupied[index] = true;                                                                            \
        (hset_ptr)->length++;                                                                                          \
        break;                                                                                                         \
      }                                                                                                                \
      if (nya_memcmp(&(hset_ptr)->items[index], &item_var, sizeof(item_var)) == 0) {                                   \
        found = true;                                                                                                  \
        break;                                                                                                         \
      }                                                                                                                \
      index = (index + 1) % (hset_ptr)->capacity;                                                                      \
      iterations++;                                                                                                    \
    }                                                                                                                  \
    (void)found;                                                                                                       \
  })

#define nya_hset_resize_and_rehash(hset_ptr, new_capacity)                                                             \
  ({                                                                                                                   \
    nya_assert((new_capacity) >= (hset_ptr)->length);                                                                  \
    typeof(*(hset_ptr)) old_hset = *(hset_ptr);                                                                        \
                                                                                                                       \
    (hset_ptr)->items    = nya_arena_alloc((hset_ptr)->arena, (new_capacity) * sizeof(*(hset_ptr)->items));            \
    (hset_ptr)->occupied = nya_arena_alloc((hset_ptr)->arena, (new_capacity) * sizeof(b8));                            \
    nya_memset((hset_ptr)->occupied, 0, (new_capacity) * sizeof(b8));                                                  \
    (hset_ptr)->capacity = (new_capacity);                                                                             \
    (hset_ptr)->length   = 0;                                                                                          \
                                                                                                                       \
    for (u64 i = 0; i < old_hset.capacity; i++) {                                                                      \
      if (!old_hset.occupied[i]) continue;                                                                             \
      nya_hset_insert_unchecked(hset_ptr, old_hset.items[i]);                                                          \
    }                                                                                                                  \
                                                                                                                       \
    nya_arena_free((hset_ptr)->arena, old_hset.items, sizeof(*old_hset.items) * old_hset.capacity);                    \
    nya_arena_free((hset_ptr)->arena, old_hset.occupied, sizeof(*old_hset.occupied) * old_hset.capacity);              \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ACCESS MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_hset_contains(hset_ptr, item)                                                                              \
  ({                                                                                                                   \
    nya_assert_type_match(item, (hset_ptr)->items[0]);                                                                 \
    typeof(item) item_var   = item;                                                                                    \
    bool         contains   = false;                                                                                   \
    u64          index      = nya_hash_fnv1a(&item_var, sizeof(item_var)) % (hset_ptr)->capacity;                      \
    u64          iterations = 0;                                                                                       \
    while (iterations < (hset_ptr)->capacity) {                                                                        \
      if (!(hset_ptr)->occupied[index]) break;                                                                         \
      if (nya_memcmp(&(hset_ptr)->items[index], &item_var, sizeof(item_var)) == 0) {                                   \
        contains = true;                                                                                               \
        break;                                                                                                         \
      }                                                                                                                \
      index = (index + 1) % (hset_ptr)->capacity;                                                                      \
      iterations++;                                                                                                    \
    }                                                                                                                  \
    contains;                                                                                                          \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * INSERT / REMOVE MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_hset_insert(hset_ptr, item)                                                                                \
  ({                                                                                                                   \
    nya_assert_type_match(item, (hset_ptr)->items[0]);                                                                 \
    if (((f32)((hset_ptr)->length + 1) / (f32)(hset_ptr)->capacity) > _NYA_HASHSET_LOAD_FACTOR) {                      \
      nya_hset_resize_and_rehash(hset_ptr, (hset_ptr)->capacity * 2);                                                  \
    }                                                                                                                  \
    typeof(item) _insert_item = item;                                                                                  \
    u64          _insert_idx  = nya_hash_fnv1a(&_insert_item, sizeof(_insert_item)) % (hset_ptr)->capacity;            \
    bool         _found_dup   = false;                                                                                 \
    while ((hset_ptr)->occupied[_insert_idx]) {                                                                        \
      if (nya_memcmp(&(hset_ptr)->items[_insert_idx], &_insert_item, sizeof(_insert_item)) == 0) {                     \
        _found_dup = true;                                                                                             \
        break;                                                                                                         \
      }                                                                                                                \
      _insert_idx = (_insert_idx + 1) % (hset_ptr)->capacity;                                                          \
    }                                                                                                                  \
    if (!_found_dup) {                                                                                                 \
      (hset_ptr)->items[_insert_idx]    = _insert_item;                                                                \
      (hset_ptr)->occupied[_insert_idx] = true;                                                                        \
      (hset_ptr)->length++;                                                                                            \
    }                                                                                                                  \
  })

#define nya_hset_remove(hset_ptr, item)                                                                                \
  ({                                                                                                                   \
    nya_assert_type_match(item, (hset_ptr)->items[0]);                                                                 \
    typeof(item) item_var   = item;                                                                                    \
    u64          index      = nya_hash_fnv1a(&item_var, sizeof(item_var)) % (hset_ptr)->capacity;                      \
    u64          iterations = 0;                                                                                       \
    while (iterations < (hset_ptr)->capacity) {                                                                        \
      if (!(hset_ptr)->occupied[index]) break;                                                                         \
      if (nya_memcmp(&(hset_ptr)->items[index], &item_var, sizeof(item_var)) == 0) {                                   \
        (hset_ptr)->occupied[index] = false;                                                                           \
        (hset_ptr)->length--;                                                                                          \
        /* Rehash subsequent entries in the probe chain */                                                             \
        u64 _rehash_idx = (index + 1) % (hset_ptr)->capacity;                                                          \
        while ((hset_ptr)->occupied[_rehash_idx]) {                                                                    \
          typeof((hset_ptr)->items[0]) _rehash_item = (hset_ptr)->items[_rehash_idx];                                  \
          (hset_ptr)->occupied[_rehash_idx]         = false;                                                           \
          (hset_ptr)->length--;                                                                                        \
          nya_hset_insert_unchecked(hset_ptr, _rehash_item);                                                           \
          _rehash_idx = (_rehash_idx + 1) % (hset_ptr)->capacity;                                                      \
        }                                                                                                              \
        break;                                                                                                         \
      }                                                                                                                \
      index = (index + 1) % (hset_ptr)->capacity;                                                                      \
      iterations++;                                                                                                    \
    }                                                                                                                  \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * SET OPERATION MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_hset_union(dest_hset_ptr, src_hset_ptr)                                                                    \
  do {                                                                                                                 \
    for (u64 _union_idx = 0; _union_idx < (src_hset_ptr)->capacity; _union_idx++) {                                    \
      if ((src_hset_ptr)->occupied[_union_idx]) { nya_hset_insert(dest_hset_ptr, (src_hset_ptr)->items[_union_idx]); } \
    }                                                                                                                  \
  } while (0)

#define nya_hset_intersection(dest_hset_ptr, src_hset_ptr)                                                             \
  do {                                                                                                                 \
    for (u64 _inter_idx = 0; _inter_idx < (dest_hset_ptr)->capacity; _inter_idx++) {                                   \
      if ((dest_hset_ptr)->occupied[_inter_idx]) {                                                                     \
        typeof((dest_hset_ptr)->items[0]) _inter_item = (dest_hset_ptr)->items[_inter_idx];                            \
        if (!nya_hset_contains(src_hset_ptr, _inter_item)) { nya_hset_remove(dest_hset_ptr, _inter_item); }            \
      }                                                                                                                \
    }                                                                                                                  \
  } while (0)

#define nya_hset_difference(dest_hset_ptr, src_hset_ptr)                                                               \
  do {                                                                                                                 \
    for (u64 _diff_idx = 0; _diff_idx < (src_hset_ptr)->capacity; _diff_idx++) {                                       \
      if ((src_hset_ptr)->occupied[_diff_idx]) {                                                                       \
        typeof((src_hset_ptr)->items[0]) _diff_item = (src_hset_ptr)->items[_diff_idx];                                \
        if (nya_hset_contains(dest_hset_ptr, _diff_item)) { nya_hset_remove(dest_hset_ptr, _diff_item); }              \
      }                                                                                                                \
    }                                                                                                                  \
  } while (0)

#define nya_hset_symmetric_difference(dest_hset_ptr, src_hset_ptr)                                                     \
  do {                                                                                                                 \
    for (u64 _sym_idx = 0; _sym_idx < (src_hset_ptr)->capacity; _sym_idx++) {                                          \
      if ((src_hset_ptr)->occupied[_sym_idx]) {                                                                        \
        typeof((src_hset_ptr)->items[0]) _sym_item = (src_hset_ptr)->items[_sym_idx];                                  \
        if (nya_hset_contains(dest_hset_ptr, _sym_item)) {                                                             \
          nya_hset_remove(dest_hset_ptr, _sym_item);                                                                   \
        } else {                                                                                                       \
          nya_hset_insert(dest_hset_ptr, _sym_item);                                                                   \
        }                                                                                                              \
      }                                                                                                                \
    }                                                                                                                  \
  } while (0)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * MEMORY MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_hset_copy(hset_ptr)                                                                                        \
  ({                                                                                                                   \
    typeof(*(hset_ptr)) copy = {                                                                                       \
        .length   = (hset_ptr)->length,                                                                                \
        .capacity = (hset_ptr)->capacity,                                                                              \
        .arena    = (hset_ptr)->arena,                                                                                 \
        .items =                                                                                                       \
            nya_arena_copy((hset_ptr)->arena, (hset_ptr)->items, sizeof(*(hset_ptr)->items) * (hset_ptr)->capacity),   \
        .occupied = nya_arena_copy(                                                                                    \
            (hset_ptr)->arena,                                                                                         \
            (hset_ptr)->occupied,                                                                                      \
            sizeof(*(hset_ptr)->occupied) * (hset_ptr)->capacity                                                       \
        ),                                                                                                             \
    };                                                                                                                 \
    copy;                                                                                                              \
  })

#define nya_hset_move(hset_ptr, new_arena_ptr)                                                                         \
  ({                                                                                                                   \
    nya_assert_type_match(new_arena_ptr, (hset_ptr)->arena);                                                           \
    *(hset_ptr) = (typeof(*(hset_ptr))){.items = nya_arena_move(                                                       \
                                            (hset_ptr)->arena,                                                         \
                                            new_arena_ptr,                                                             \
                                            (hset_ptr)->items,                                                         \
                                            sizeof(*(hset_ptr)->items) * (hset_ptr)->capacity                          \
                                        ),                                                                             \
                                        .occupied = nya_arena_move(                                                    \
                                            (hset_ptr)->arena,                                                         \
                                            new_arena_ptr,                                                             \
                                            (hset_ptr)->occupied,                                                      \
                                            sizeof(*(hset_ptr)->occupied) * (hset_ptr)->capacity                       \
                                        ),                                                                             \
                                        .length   = (hset_ptr)->length,                                                \
                                        .capacity = (hset_ptr)->capacity,                                              \
                                        .arena    = new_arena_ptr};                                                       \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ITERATOR MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_hset_foreach(hset_ptr, item_name)                                                                          \
  for (u64 _hset_foreach_idx = 0; _hset_foreach_idx < (hset_ptr)->capacity; _hset_foreach_idx++)                       \
    if ((hset_ptr)->occupied[_hset_foreach_idx])                                                                       \
      for (typeof((hset_ptr)->items[0]) item_name = (hset_ptr)->items[_hset_foreach_idx]; (void)item_name, true; break)
