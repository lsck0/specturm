/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

nya_derive_hset(u32);
nya_derive_hset(s32);

s32 main(void) {
  NYA_Arena arena = nya_arena_create(.name = "test_hset");

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: basic hashset creation
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet set = nya_hset_create(&arena, u32);
  nya_assert(set.length == 0);
  nya_assert(set.capacity == _NYA_HASHSET_DEFAULT_CAPACITY);
  nya_assert(set.items != nullptr);
  nya_assert(set.occupied != nullptr);
  nya_assert(set.arena == &arena);
  nya_hset_destroy(&set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: creation with custom capacity
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet set_cap = nya_hset_create_with_capacity(&arena, u32, 128);
  nya_assert(set_cap.length == 0);
  nya_assert(set_cap.capacity == 128);
  nya_hset_destroy(&set_cap);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_insert and nya_hset_contains
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet insert_set = nya_hset_create(&arena, u32);
  nya_hset_insert(&insert_set, 10U);
  nya_assert(insert_set.length == 1);
  nya_assert(nya_hset_contains(&insert_set, 10U) == true);

  nya_hset_insert(&insert_set, 20U);
  nya_hset_insert(&insert_set, 30U);
  nya_assert(insert_set.length == 3);
  nya_assert(nya_hset_contains(&insert_set, 10U) == true);
  nya_assert(nya_hset_contains(&insert_set, 20U) == true);
  nya_assert(nya_hset_contains(&insert_set, 30U) == true);
  nya_hset_destroy(&insert_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_contains returns false for missing items
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet contains_set = nya_hset_create(&arena, u32);
  nya_hset_insert(&contains_set, 5U);
  nya_assert(nya_hset_contains(&contains_set, 5U) == true);
  nya_assert(nya_hset_contains(&contains_set, 99U) == false);
  nya_assert(nya_hset_contains(&contains_set, 0U) == false);
  nya_hset_destroy(&contains_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_remove
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet remove_set = nya_hset_create(&arena, u32);
  nya_hset_insert(&remove_set, 1U);
  nya_hset_insert(&remove_set, 2U);
  nya_hset_insert(&remove_set, 3U);
  nya_assert(remove_set.length == 3);

  nya_hset_remove(&remove_set, 2U);
  nya_assert(remove_set.length == 2);
  nya_assert(nya_hset_contains(&remove_set, 1U) == true);
  nya_assert(nya_hset_contains(&remove_set, 2U) == false);
  nya_assert(nya_hset_contains(&remove_set, 3U) == true);
  nya_hset_destroy(&remove_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_remove first and last
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet remove_first_last = nya_hset_create(&arena, u32);
  nya_hset_insert(&remove_first_last, 100U);
  nya_hset_insert(&remove_first_last, 200U);
  nya_hset_insert(&remove_first_last, 300U);

  nya_hset_remove(&remove_first_last, 100U);
  nya_assert(remove_first_last.length == 2);
  nya_assert(nya_hset_contains(&remove_first_last, 100U) == false);

  nya_hset_remove(&remove_first_last, 300U);
  nya_assert(remove_first_last.length == 1);
  nya_assert(nya_hset_contains(&remove_first_last, 300U) == false);
  nya_assert(nya_hset_contains(&remove_first_last, 200U) == true);
  nya_hset_destroy(&remove_first_last);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_clear
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet clear_set = nya_hset_create(&arena, u32);
  nya_hset_insert(&clear_set, 1U);
  nya_hset_insert(&clear_set, 2U);
  nya_hset_insert(&clear_set, 3U);
  nya_assert(clear_set.length == 3);

  nya_hset_clear(&clear_set);
  nya_assert(clear_set.length == 0);
  nya_assert(nya_hset_contains(&clear_set, 1U) == false);
  nya_assert(nya_hset_contains(&clear_set, 2U) == false);
  nya_assert(nya_hset_contains(&clear_set, 3U) == false);
  nya_assert(clear_set.capacity == _NYA_HASHSET_DEFAULT_CAPACITY);
  nya_hset_destroy(&clear_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: automatic resize on load factor exceeded
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet resize_set = nya_hset_create_with_capacity(&arena, u32, 8);
  nya_assert(resize_set.capacity == 8);
  for (u32 i = 0; i < 10; ++i) { nya_hset_insert(&resize_set, i * 10); }
  nya_assert(resize_set.length == 10);
  nya_assert(resize_set.capacity >= 10);
  for (u32 i = 0; i < 10; ++i) { nya_assert(nya_hset_contains(&resize_set, i * 10) == true); }
  nya_hset_destroy(&resize_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: many insertions and removals
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet stress_set = nya_hset_create(&arena, u32);
  for (u32 i = 0; i < 100; ++i) { nya_hset_insert(&stress_set, i); }
  nya_assert(stress_set.length == 100);

  for (u32 i = 0; i < 50; ++i) { nya_hset_remove(&stress_set, i * 2); }
  nya_assert(stress_set.length == 50);

  for (u32 i = 0; i < 50; ++i) {
    nya_assert(nya_hset_contains(&stress_set, i * 2) == false);
    nya_assert(nya_hset_contains(&stress_set, i * 2 + 1) == true);
  }
  nya_hset_destroy(&stress_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_copy
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet orig_set = nya_hset_create(&arena, u32);
  nya_hset_insert(&orig_set, 1U);
  nya_hset_insert(&orig_set, 2U);
  nya_hset_insert(&orig_set, 3U);

  u32HSet copy_set = nya_hset_copy(&orig_set);
  nya_assert(copy_set.length == orig_set.length);
  nya_assert(copy_set.capacity == orig_set.capacity);
  nya_assert(nya_hset_contains(&copy_set, 1U) == true);
  nya_assert(nya_hset_contains(&copy_set, 2U) == true);
  nya_assert(nya_hset_contains(&copy_set, 3U) == true);

  nya_hset_remove(&orig_set, 1U);
  nya_assert(nya_hset_contains(&orig_set, 1U) == false);
  nya_assert(nya_hset_contains(&copy_set, 1U) == true);
  nya_hset_destroy(&orig_set);
  nya_hset_destroy(&copy_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_move
  // ─────────────────────────────────────────────────────────────────────────────
  NYA_Arena arena2 = nya_arena_create(.name = "test_hset_move");
  u32HSet move_set = nya_hset_create(&arena, u32);
  nya_hset_insert(&move_set, 10U);
  nya_hset_insert(&move_set, 20U);

  nya_hset_move(&move_set, &arena2);
  nya_assert(move_set.arena == &arena2);
  nya_assert(move_set.length == 2);
  nya_assert(nya_hset_contains(&move_set, 10U) == true);
  nya_assert(nya_hset_contains(&move_set, 20U) == true);
  nya_hset_destroy(&move_set);
  nya_arena_destroy(&arena2);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: manual iteration over items
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet iter_set = nya_hset_create(&arena, u32);
  nya_hset_insert(&iter_set, 1U);
  nya_hset_insert(&iter_set, 2U);
  nya_hset_insert(&iter_set, 3U);

  u64 item_sum = 0;
  for (u64 i = 0; i < iter_set.capacity; i++) {
    if (iter_set.occupied[i]) { item_sum += iter_set.items[i]; }
  }
  nya_assert(item_sum == 6);
  nya_hset_destroy(&iter_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: signed integer items
  // ─────────────────────────────────────────────────────────────────────────────
  s32HSet signed_set = nya_hset_create(&arena, s32);
  nya_hset_insert(&signed_set, -5);
  nya_hset_insert(&signed_set, 0);
  nya_hset_insert(&signed_set, 5);
  nya_assert(signed_set.length == 3);
  nya_assert(nya_hset_contains(&signed_set, -5) == true);
  nya_assert(nya_hset_contains(&signed_set, 0) == true);
  nya_assert(nya_hset_contains(&signed_set, 5) == true);
  nya_assert(nya_hset_contains(&signed_set, -10) == false);
  nya_hset_destroy(&signed_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: remove non-existent item (should not crash)
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet remove_nonexist = nya_hset_create(&arena, u32);
  nya_hset_insert(&remove_nonexist, 1U);
  nya_hset_remove(&remove_nonexist, 999U);
  nya_assert(remove_nonexist.length == 1);
  nya_assert(nya_hset_contains(&remove_nonexist, 1U) == true);
  nya_hset_destroy(&remove_nonexist);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: empty hashset operations
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet empty_set = nya_hset_create(&arena, u32);
  nya_assert(empty_set.length == 0);
  nya_assert(nya_hset_contains(&empty_set, 1U) == false);
  nya_hset_remove(&empty_set, 1U);
  nya_assert(empty_set.length == 0);
  nya_hset_destroy(&empty_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: duplicate insertion (set behavior - should deduplicate)
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet dup_set = nya_hset_create(&arena, u32);
  nya_hset_insert(&dup_set, 42U);
  nya_hset_insert(&dup_set, 42U);
  nya_hset_insert(&dup_set, 42U);
  nya_assert(dup_set.length == 1);
  nya_assert(nya_hset_contains(&dup_set, 42U) == true);
  nya_hset_destroy(&dup_set);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: destroy resets hashset state
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet destroy_set = nya_hset_create(&arena, u32);
  nya_hset_insert(&destroy_set, 1U);
  nya_hset_insert(&destroy_set, 2U);
  nya_hset_destroy(&destroy_set);
  nya_assert(destroy_set.items == nullptr);
  nya_assert(destroy_set.occupied == nullptr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_union
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet union_a = nya_hset_create(&arena, u32);
  u32HSet union_b = nya_hset_create(&arena, u32);
  nya_hset_insert(&union_a, 1U);
  nya_hset_insert(&union_a, 2U);
  nya_hset_insert(&union_a, 3U);
  nya_hset_insert(&union_b, 3U);
  nya_hset_insert(&union_b, 4U);
  nya_hset_insert(&union_b, 5U);
  nya_hset_union(&union_a, &union_b);
  nya_assert(union_a.length == 5);
  nya_assert(nya_hset_contains(&union_a, 1U) == true);
  nya_assert(nya_hset_contains(&union_a, 2U) == true);
  nya_assert(nya_hset_contains(&union_a, 3U) == true);
  nya_assert(nya_hset_contains(&union_a, 4U) == true);
  nya_assert(nya_hset_contains(&union_a, 5U) == true);
  nya_hset_destroy(&union_a);
  nya_hset_destroy(&union_b);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_intersection
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet inter_a = nya_hset_create(&arena, u32);
  u32HSet inter_b = nya_hset_create(&arena, u32);
  nya_hset_insert(&inter_a, 1U);
  nya_hset_insert(&inter_a, 2U);
  nya_hset_insert(&inter_a, 3U);
  nya_hset_insert(&inter_a, 4U);
  nya_hset_insert(&inter_b, 2U);
  nya_hset_insert(&inter_b, 4U);
  nya_hset_insert(&inter_b, 6U);
  nya_hset_intersection(&inter_a, &inter_b);
  nya_assert(inter_a.length == 2);
  nya_assert(nya_hset_contains(&inter_a, 1U) == false);
  nya_assert(nya_hset_contains(&inter_a, 2U) == true);
  nya_assert(nya_hset_contains(&inter_a, 3U) == false);
  nya_assert(nya_hset_contains(&inter_a, 4U) == true);
  nya_hset_destroy(&inter_a);
  nya_hset_destroy(&inter_b);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_difference
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet diff_a = nya_hset_create(&arena, u32);
  u32HSet diff_b = nya_hset_create(&arena, u32);
  nya_hset_insert(&diff_a, 1U);
  nya_hset_insert(&diff_a, 2U);
  nya_hset_insert(&diff_a, 3U);
  nya_hset_insert(&diff_a, 4U);
  nya_hset_insert(&diff_b, 2U);
  nya_hset_insert(&diff_b, 4U);
  nya_hset_difference(&diff_a, &diff_b);
  nya_assert(diff_a.length == 2);
  nya_assert(nya_hset_contains(&diff_a, 1U) == true);
  nya_assert(nya_hset_contains(&diff_a, 2U) == false);
  nya_assert(nya_hset_contains(&diff_a, 3U) == true);
  nya_assert(nya_hset_contains(&diff_a, 4U) == false);
  nya_hset_destroy(&diff_a);
  nya_hset_destroy(&diff_b);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_hset_symmetric_difference
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet sym_a = nya_hset_create(&arena, u32);
  u32HSet sym_b = nya_hset_create(&arena, u32);
  nya_hset_insert(&sym_a, 1U);
  nya_hset_insert(&sym_a, 2U);
  nya_hset_insert(&sym_a, 3U);
  nya_hset_insert(&sym_b, 2U);
  nya_hset_insert(&sym_b, 3U);
  nya_hset_insert(&sym_b, 4U);
  nya_hset_symmetric_difference(&sym_a, &sym_b);
  nya_assert(sym_a.length == 2);
  nya_assert(nya_hset_contains(&sym_a, 1U) == true);
  nya_assert(nya_hset_contains(&sym_a, 2U) == false);
  nya_assert(nya_hset_contains(&sym_a, 3U) == false);
  nya_assert(nya_hset_contains(&sym_a, 4U) == true);
  nya_hset_destroy(&sym_a);
  nya_hset_destroy(&sym_b);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: set operations with empty sets
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet empty_a = nya_hset_create(&arena, u32);
  u32HSet empty_b = nya_hset_create(&arena, u32);
  nya_hset_insert(&empty_a, 1U);
  nya_hset_insert(&empty_a, 2U);
  nya_hset_union(&empty_a, &empty_b);
  nya_assert(empty_a.length == 2);
  nya_hset_intersection(&empty_a, &empty_b);
  nya_assert(empty_a.length == 0);
  nya_hset_destroy(&empty_a);
  nya_hset_destroy(&empty_b);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: set operations with identical sets
  // ─────────────────────────────────────────────────────────────────────────────
  u32HSet ident_a = nya_hset_create(&arena, u32);
  u32HSet ident_b = nya_hset_create(&arena, u32);
  nya_hset_insert(&ident_a, 1U);
  nya_hset_insert(&ident_a, 2U);
  nya_hset_insert(&ident_b, 1U);
  nya_hset_insert(&ident_b, 2U);
  nya_hset_symmetric_difference(&ident_a, &ident_b);
  nya_assert(ident_a.length == 0);
  nya_hset_destroy(&ident_a);
  nya_hset_destroy(&ident_b);

  nya_arena_destroy(&arena);
  return 0;
}
