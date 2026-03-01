/**
 * @file base_dict.h
 *
 * Hashmap but with string keys. The essential difference is, that a nya_derive_hmap(NYA_CString, T) would
 * compare the strings by pointer equality not value equality. Hence this mostly wraps base_hmap.
 *
 * API Overview:
 * - nya_dict_create(arena_ptr, value_type)
 * - nya_dict_create_with_capacity(arena_ptr, value_type, initial_capacity)
 * - nya_dict_create_on_stack(arena_ptr, value_type)
 * - nya_dict_create_with_capacity_on_stack(arena_ptr, value_type, initial_capacity)
 * - nya_dict_clear(dict_ptr)
 * - nya_dict_destroy(dict_ptr)
 * - nya_dict_destroy_on_stack(dict_ptr)
 * - nya_dict_resize_and_rehash(dict_ptr, new_capacity)
 * - nya_dict_contains(dict_ptr, key)
 * - nya_dict_get(dict_ptr, key)
 * - nya_dict_set(dict_ptr, key, value)
 * - nya_dict_remove(dict_ptr, key)
 * - nya_dict_copy(dict_ptr)
 * - nya_dict_move(dict_ptr, new_arena_ptr)
 * - nya_dict_foreach_key(dict_ptr, key_name)
 * - nya_dict_foreach_value(dict_ptr, value_name)
 *
 * Example:
 * ```c
 * typedef struct {
 *   u32   id;
 *   char* name;
 * } Player;
 * nya_derive_dict(Player);
 *
 * NYA_Arena* arena = nya_arena_create(...);
 * NYA_CString_Player_Dict* players = nya_dict_create(arena, Player);
 *
 * nya_dict_set(players, "alice", (Player){ .id = 1, .name = "Alice" });
 * nya_dict_set(players, "bob",   (Player){ .id = 2, .name = "Bob" });
 *
 * Player* alice = nya_dict_get(players, "alice");
 * if (alice != nullptr) {
 *   ...
 * }
 *
 * nya_arena_destroy(arena);
 * ```
 * */
#pragma once

#include "nyangine/base/base_hmap.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_derive_dict(value_type)                                                                                                                  \
  nya_derive_hmap(NYA_CString, value_type);                                                                                                          \
  typedef NYA_CString_##value_type##_HMap NYA_CString_##value_type##_Dict;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * CREATION MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_dict_create(arena_ptr, value_type) nya_hmap_create(arena_ptr, NYA_CString, value_type)
#define nya_dict_create_with_capacity(arena_ptr, value_type, initial_capacity)                                                                       \
  nya_hmap_create_with_capacity(arena_ptr, NYA_CString, value_type, initial_capacity)
#define nya_dict_create_on_stack(arena_ptr, value_type) nya_hmap_create_on_stack(arena_ptr, NYA_CString, value_type)
#define nya_dict_create_with_capacity_on_stack(arena_ptr, value_type, initial_capacity)                                                              \
  nya_hmap_create_with_capacity_on_stack(arena_ptr, NYA_CString, value_type, initial_capacity)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FORWARDS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_dict_clear(dict_ptr)                     nya_hmap_clear(dict_ptr)
#define nya_dict_destroy(dict_ptr)                   nya_hmap_destroy(dict_ptr)
#define nya_dict_destroy_on_stack(dict_ptr)          nya_hmap_destroy_on_stack(dict_ptr)
#define nya_dict_copy(dict_ptr)                      nya_hmap_copy(dict_ptr)
#define nya_dict_move(dict_ptr, new_arena_ptr)       nya_hmap_move(dict_ptr, new_arena_ptr)
#define nya_dict_foreach_key(dict_ptr, key_name)     nya_hmap_foreach_key (dict_ptr, key_name)
#define nya_dict_foreach_value(dict_ptr, value_name) nya_hmap_foreach_value (dict_ptr, value_name)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * RESIZE AND REHASH MACRO
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define _nya_dict_set_unchecked(dict_ptr, key, value)                                                                                                \
  ({                                                                                                                                                 \
    nya_assert_type_match(value, (dict_ptr)->values[0]);                                                                                             \
    NYA_CString   key_var    = key;                                                                                                                  \
    typeof(value) value_var  = value;                                                                                                                \
    u64           index      = nya_hash_fnv1a(key_var) % (dict_ptr)->capacity;                                                                       \
    u64           iterations = 0;                                                                                                                    \
    b8            updated    = false;                                                                                                                \
    while (iterations < (dict_ptr)->capacity) {                                                                                                      \
      if (!(dict_ptr)->occupied[index]) {                                                                                                            \
        (dict_ptr)->keys[index]     = key_var;                                                                                                       \
        (dict_ptr)->values[index]   = value_var;                                                                                                     \
        (dict_ptr)->occupied[index] = true;                                                                                                          \
        (dict_ptr)->length++;                                                                                                                        \
        break;                                                                                                                                       \
      }                                                                                                                                              \
      if (strcmp((dict_ptr)->keys[index], key_var) == 0) {                                                                                           \
        (dict_ptr)->values[index] = value_var;                                                                                                       \
        updated                   = true;                                                                                                            \
        break;                                                                                                                                       \
      }                                                                                                                                              \
      index = (index + 1) % (dict_ptr)->capacity;                                                                                                    \
      iterations++;                                                                                                                                  \
    }                                                                                                                                                \
    (void)updated;                                                                                                                                   \
  })

#define nya_dict_resize_and_rehash(dict_ptr, new_capacity)                                                                                           \
  ({                                                                                                                                                 \
    typeof((dict_ptr)->keys[0])*   old_keys     = (dict_ptr)->keys;                                                                                  \
    typeof((dict_ptr)->values[0])* old_values   = (dict_ptr)->values;                                                                                \
    b8*                            old_occupied = (dict_ptr)->occupied;                                                                              \
    u64                            old_capacity = (dict_ptr)->capacity;                                                                              \
                                                                                                                                                     \
    (dict_ptr)->keys     = nya_arena_alloc((dict_ptr)->arena, (new_capacity) * sizeof(*(dict_ptr)->keys));                                           \
    (dict_ptr)->values   = nya_arena_alloc((dict_ptr)->arena, (new_capacity) * sizeof(*(dict_ptr)->values));                                         \
    (dict_ptr)->occupied = nya_arena_alloc((dict_ptr)->arena, (new_capacity) * sizeof(b8));                                                          \
    nya_memset((dict_ptr)->occupied, 0, (new_capacity) * sizeof(b8));                                                                                \
    (dict_ptr)->capacity = (new_capacity);                                                                                                           \
    (dict_ptr)->length   = 0;                                                                                                                        \
                                                                                                                                                     \
    for (u64 i = 0; i < old_capacity; i++) {                                                                                                         \
      if (!old_occupied[i]) continue;                                                                                                                \
      _nya_dict_set_unchecked(dict_ptr, old_keys[i], old_values[i]);                                                                                 \
    }                                                                                                                                                \
                                                                                                                                                     \
    nya_arena_free((dict_ptr)->arena, old_keys, sizeof(*old_keys) * old_capacity);                                                                   \
    nya_arena_free((dict_ptr)->arena, old_values, sizeof(*old_values) * old_capacity);                                                               \
    nya_arena_free((dict_ptr)->arena, old_occupied, sizeof(*old_occupied) * old_capacity);                                                           \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ACCESS MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_dict_contains(dict_ptr, key)                                                                                                             \
  ({                                                                                                                                                 \
    NYA_CString key_var    = key;                                                                                                                    \
    bool        contains   = false;                                                                                                                  \
    u64         index      = nya_hash_fnv1a(key_var) % (dict_ptr)->capacity;                                                                         \
    u64         iterations = 0;                                                                                                                      \
    while (iterations < (dict_ptr)->capacity) {                                                                                                      \
      if (!(dict_ptr)->occupied[index]) break;                                                                                                       \
      if (strcmp((dict_ptr)->keys[index], key_var) == 0) {                                                                                           \
        contains = true;                                                                                                                             \
        break;                                                                                                                                       \
      }                                                                                                                                              \
      index = (index + 1) % (dict_ptr)->capacity;                                                                                                    \
      iterations++;                                                                                                                                  \
    }                                                                                                                                                \
    contains;                                                                                                                                        \
  })

#define nya_dict_get(dict_ptr, key)                                                                                                                  \
  ({                                                                                                                                                 \
    NYA_CString key_var    = key;                                                                                                                    \
    bool        contains   = false;                                                                                                                  \
    u64         index      = nya_hash_fnv1a(key_var) % (dict_ptr)->capacity;                                                                         \
    u64         iterations = 0;                                                                                                                      \
    while (iterations < (dict_ptr)->capacity) {                                                                                                      \
      if (!(dict_ptr)->occupied[index]) break;                                                                                                       \
      if (strcmp((dict_ptr)->keys[index], key_var) == 0) {                                                                                           \
        contains = true;                                                                                                                             \
        break;                                                                                                                                       \
      }                                                                                                                                              \
      index = (index + 1) % (dict_ptr)->capacity;                                                                                                    \
      iterations++;                                                                                                                                  \
    }                                                                                                                                                \
    contains ? &(dict_ptr)->values[index] : nullptr;                                                                                                 \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * INSERT / REMOVE MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_dict_set(dict_ptr, key, value)                                                                                                           \
  ({                                                                                                                                                 \
    nya_assert_type_match(value, (dict_ptr)->values[0]);                                                                                             \
    if (((f32)((dict_ptr)->length + 1) / (f32)(dict_ptr)->capacity) > _NYA_HASHMAP_LOAD_FACTOR) {                                                    \
      nya_dict_resize_and_rehash(dict_ptr, (dict_ptr)->capacity * 2);                                                                                \
    }                                                                                                                                                \
    _nya_dict_set_unchecked(dict_ptr, key, value);                                                                                                   \
  })

#define nya_dict_remove(dict_ptr, key)                                                                                                               \
  ({                                                                                                                                                 \
    NYA_CString key_var    = key;                                                                                                                    \
    u64         index      = nya_hash_fnv1a(key_var) % (dict_ptr)->capacity;                                                                         \
    u64         iterations = 0;                                                                                                                      \
    while (iterations < (dict_ptr)->capacity) {                                                                                                      \
      if (!(dict_ptr)->occupied[index]) break;                                                                                                       \
      if (strcmp((dict_ptr)->keys[index], key_var) == 0) {                                                                                           \
        (dict_ptr)->occupied[index] = false;                                                                                                         \
        (dict_ptr)->length--;                                                                                                                        \
        u64 _rehash_idx = (index + 1) % (dict_ptr)->capacity;                                                                                        \
        while ((dict_ptr)->occupied[_rehash_idx]) {                                                                                                  \
          typeof((dict_ptr)->keys[0])   _rehash_key   = (dict_ptr)->keys[_rehash_idx];                                                               \
          typeof((dict_ptr)->values[0]) _rehash_value = (dict_ptr)->values[_rehash_idx];                                                             \
          (dict_ptr)->occupied[_rehash_idx]           = false;                                                                                       \
          (dict_ptr)->length--;                                                                                                                      \
          u64 _new_idx  = nya_hash_fnv1a(_rehash_key) % (dict_ptr)->capacity;                                                                        \
          u64 _new_iter = 0;                                                                                                                         \
          while (_new_iter < (dict_ptr)->capacity) {                                                                                                 \
            if (!(dict_ptr)->occupied[_new_idx]) {                                                                                                   \
              (dict_ptr)->keys[_new_idx]     = _rehash_key;                                                                                          \
              (dict_ptr)->values[_new_idx]   = _rehash_value;                                                                                        \
              (dict_ptr)->occupied[_new_idx] = true;                                                                                                 \
              (dict_ptr)->length++;                                                                                                                  \
              break;                                                                                                                                 \
            }                                                                                                                                        \
            _new_idx = (_new_idx + 1) % (dict_ptr)->capacity;                                                                                        \
            _new_iter++;                                                                                                                             \
          }                                                                                                                                          \
          _rehash_idx = (_rehash_idx + 1) % (dict_ptr)->capacity;                                                                                    \
        }                                                                                                                                            \
        break;                                                                                                                                       \
      }                                                                                                                                              \
      index = (index + 1) % (dict_ptr)->capacity;                                                                                                    \
      iterations++;                                                                                                                                  \
    }                                                                                                                                                \
  })
