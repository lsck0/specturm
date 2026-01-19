/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

nya_derive_hmap(u32, u64);
nya_derive_hmap(s32, s32);

s32 main(void) {
  NYA_Arena arena = nya_arena_create(.name = "test_hmap");

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: basic hashmap creation
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap map = nya_hmap_create(&arena, u32, u64);
  nya_assert(map.length == 0);
  nya_assert(map.capacity == _NYA_HASHMAP_DEFAULT_CAPACITY);
  nya_assert(map.keys != nullptr);
  nya_assert(map.values != nullptr);
  nya_assert(map.occupied != nullptr);
  nya_assert(map.arena == &arena);
  nya_hmap_destroy(&map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: creation with custom capacity
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap map_cap = nya_hmap_create_with_capacity(&arena, u32, u64, 128);
  nya_assert(map_cap.length == 0);
  nya_assert(map_cap.capacity == 128);
  nya_hmap_destroy(&map_cap);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hmap_insert and nya_hmap_get
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap insert_map = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&insert_map, 1U, 100UL);
  nya_assert(insert_map.length == 1);
  u64* val = nya_hmap_get(&insert_map, 1U);
  nya_assert(val != nullptr);
  nya_assert(*val == 100UL);

  nya_hmap_insert(&insert_map, 2U, 200UL);
  nya_hmap_insert(&insert_map, 3U, 300UL);
  nya_assert(insert_map.length == 3);
  nya_assert(*nya_hmap_get(&insert_map, 2U) == 200UL);
  nya_assert(*nya_hmap_get(&insert_map, 3U) == 300UL);
  nya_hmap_destroy(&insert_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hmap_contains
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap contains_map = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&contains_map, 10U, 1000UL);
  nya_hmap_insert(&contains_map, 20U, 2000UL);
  nya_assert(nya_hmap_contains(&contains_map, 10U) == true);
  nya_assert(nya_hmap_contains(&contains_map, 20U) == true);
  nya_assert(nya_hmap_contains(&contains_map, 30U) == false);
  nya_assert(nya_hmap_contains(&contains_map, 0U) == false);
  nya_hmap_destroy(&contains_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hmap_get returns nullptr for missing keys
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap get_map = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&get_map, 5U, 500UL);
  nya_assert(nya_hmap_get(&get_map, 5U) != nullptr);
  nya_assert(nya_hmap_get(&get_map, 99U) == nullptr);
  nya_hmap_destroy(&get_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hmap_remove
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap remove_map = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&remove_map, 1U, 10UL);
  nya_hmap_insert(&remove_map, 2U, 20UL);
  nya_hmap_insert(&remove_map, 3U, 30UL);
  nya_assert(remove_map.length == 3);

  nya_hmap_remove(&remove_map, 2U);
  nya_assert(remove_map.length == 2);
  nya_assert(nya_hmap_contains(&remove_map, 1U) == true);
  nya_assert(nya_hmap_contains(&remove_map, 2U) == false);
  nya_assert(nya_hmap_contains(&remove_map, 3U) == true);
  nya_assert(*nya_hmap_get(&remove_map, 1U) == 10UL);
  nya_assert(*nya_hmap_get(&remove_map, 3U) == 30UL);
  nya_hmap_destroy(&remove_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hmap_remove first and last
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap remove_first_last = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&remove_first_last, 100U, 1UL);
  nya_hmap_insert(&remove_first_last, 200U, 2UL);
  nya_hmap_insert(&remove_first_last, 300U, 3UL);

  nya_hmap_remove(&remove_first_last, 100U);
  nya_assert(remove_first_last.length == 2);
  nya_assert(nya_hmap_contains(&remove_first_last, 100U) == false);

  nya_hmap_remove(&remove_first_last, 300U);
  nya_assert(remove_first_last.length == 1);
  nya_assert(nya_hmap_contains(&remove_first_last, 300U) == false);
  nya_assert(nya_hmap_contains(&remove_first_last, 200U) == true);
  nya_hmap_destroy(&remove_first_last);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hmap_clear
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap clear_map = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&clear_map, 1U, 1UL);
  nya_hmap_insert(&clear_map, 2U, 2UL);
  nya_hmap_insert(&clear_map, 3U, 3UL);
  nya_assert(clear_map.length == 3);

  nya_hmap_clear(&clear_map);
  nya_assert(clear_map.length == 0);
  nya_assert(nya_hmap_contains(&clear_map, 1U) == false);
  nya_assert(nya_hmap_contains(&clear_map, 2U) == false);
  nya_assert(nya_hmap_contains(&clear_map, 3U) == false);
  nya_assert(clear_map.capacity == _NYA_HASHMAP_DEFAULT_CAPACITY);
  nya_hmap_destroy(&clear_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: automatic resize on load factor exceeded
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap resize_map = nya_hmap_create_with_capacity(&arena, u32, u64, 8);
  nya_assert(resize_map.capacity == 8);
  for (u32 i = 0; i < 10; ++i) { nya_hmap_insert(&resize_map, i, (u64)(i * 100)); }
  nya_assert(resize_map.length == 10);
  nya_assert(resize_map.capacity >= 10);
  for (u32 i = 0; i < 10; ++i) {
    nya_assert(nya_hmap_contains(&resize_map, i) == true);
    nya_assert(*nya_hmap_get(&resize_map, i) == (u64)(i * 100));
  }
  nya_hmap_destroy(&resize_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: many insertions and removals
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap stress_map = nya_hmap_create(&arena, u32, u64);
  for (u32 i = 0; i < 100; ++i) { nya_hmap_insert(&stress_map, i, (u64)(i * 10)); }
  nya_assert(stress_map.length == 100);

  for (u32 i = 0; i < 50; ++i) { nya_hmap_remove(&stress_map, i * 2); }
  nya_assert(stress_map.length == 50);

  for (u32 i = 0; i < 50; ++i) {
    nya_assert(nya_hmap_contains(&stress_map, i * 2) == false);
    nya_assert(nya_hmap_contains(&stress_map, i * 2 + 1) == true);
    nya_assert(*nya_hmap_get(&stress_map, i * 2 + 1) == (u64)((i * 2 + 1) * 10));
  }
  nya_hmap_destroy(&stress_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hmap_copy
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap orig_map = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&orig_map, 1U, 100UL);
  nya_hmap_insert(&orig_map, 2U, 200UL);
  nya_hmap_insert(&orig_map, 3U, 300UL);

  u32_u64_HMap copy_map = nya_hmap_copy(&orig_map);
  nya_assert(copy_map.length == orig_map.length);
  nya_assert(copy_map.capacity == orig_map.capacity);
  nya_assert(*nya_hmap_get(&copy_map, 1U) == 100UL);
  nya_assert(*nya_hmap_get(&copy_map, 2U) == 200UL);
  nya_assert(*nya_hmap_get(&copy_map, 3U) == 300UL);

  nya_hmap_remove(&orig_map, 1U);
  nya_assert(nya_hmap_contains(&orig_map, 1U) == false);
  nya_assert(nya_hmap_contains(&copy_map, 1U) == true);
  nya_hmap_destroy(&orig_map);
  nya_hmap_destroy(&copy_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hmap_move
  // ─────────────────────────────────────────────────────────────────────────────
  NYA_Arena arena2 = nya_arena_create(.name = "test_hmap_move");
  u32_u64_HMap move_map = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&move_map, 10U, 1000UL);
  nya_hmap_insert(&move_map, 20U, 2000UL);

  nya_hmap_move(&move_map, &arena2);
  nya_assert(move_map.arena == &arena2);
  nya_assert(move_map.length == 2);
  nya_assert(*nya_hmap_get(&move_map, 10U) == 1000UL);
  nya_assert(*nya_hmap_get(&move_map, 20U) == 2000UL);
  nya_hmap_destroy(&move_map);
  nya_arena_destroy(&arena2);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: manual iteration over keys and values
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap iter_map = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&iter_map, 1U, 100UL);
  nya_hmap_insert(&iter_map, 2U, 200UL);
  nya_hmap_insert(&iter_map, 3U, 300UL);

  u64 key_sum = 0;
  u64 val_sum = 0;
  for (u64 i = 0; i < iter_map.capacity; i++) {
    if (iter_map.occupied[i]) {
      key_sum += iter_map.keys[i];
      val_sum += iter_map.values[i];
    }
  }
  nya_assert(key_sum == 6);
  nya_assert(val_sum == 600);
  nya_hmap_destroy(&iter_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: signed integer keys
  // ─────────────────────────────────────────────────────────────────────────────
  s32_s32_HMap signed_map = nya_hmap_create(&arena, s32, s32);
  nya_hmap_insert(&signed_map, -5, 50);
  nya_hmap_insert(&signed_map, 0, 0);
  nya_hmap_insert(&signed_map, 5, -50);
  nya_assert(signed_map.length == 3);
  nya_assert(*nya_hmap_get(&signed_map, -5) == 50);
  nya_assert(*nya_hmap_get(&signed_map, 0) == 0);
  nya_assert(*nya_hmap_get(&signed_map, 5) == -50);
  nya_hmap_destroy(&signed_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: remove non-existent key (should not crash)
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap remove_nonexist = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&remove_nonexist, 1U, 10UL);
  nya_hmap_remove(&remove_nonexist, 999U);
  nya_assert(remove_nonexist.length == 1);
  nya_assert(nya_hmap_contains(&remove_nonexist, 1U) == true);
  nya_hmap_destroy(&remove_nonexist);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: empty hashmap operations
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap empty_map = nya_hmap_create(&arena, u32, u64);
  nya_assert(empty_map.length == 0);
  nya_assert(nya_hmap_contains(&empty_map, 1U) == false);
  nya_assert(nya_hmap_get(&empty_map, 1U) == nullptr);
  nya_hmap_remove(&empty_map, 1U);
  nya_assert(empty_map.length == 0);
  nya_hmap_destroy(&empty_map);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: destroy resets hashmap state
  // ─────────────────────────────────────────────────────────────────────────────
  u32_u64_HMap destroy_map = nya_hmap_create(&arena, u32, u64);
  nya_hmap_insert(&destroy_map, 1U, 1UL);
  nya_hmap_insert(&destroy_map, 2U, 2UL);
  nya_hmap_destroy(&destroy_map);
  nya_assert(destroy_map.keys == nullptr);
  nya_assert(destroy_map.values == nullptr);
  nya_assert(destroy_map.occupied == nullptr);
  nya_assert(destroy_map.length == 0);
  nya_assert(destroy_map.capacity == 0);

  nya_arena_destroy(&arena);
  return 0;
}
