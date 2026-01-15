/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

typedef struct {
  s32 x;
  s32 y;
} TestPoint;
nya_derive_array(TestPoint);

static s32 compare_u32_asc(const u32* a, const u32* b) {
  if (*a < *b) return -1;
  if (*a > *b) return 1;
  return 0;
}

static s32 compare_u32_desc(const u32* a, const u32* b) {
  if (*a > *b) return -1;
  if (*a < *b) return 1;
  return 0;
}

static s32 compare_s32_asc(const s32* a, const s32* b) {
  return *a - *b;
}

s32 main(void) {
  NYA_Arena arena = nya_arena_new(.name = "test_array");

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: basic array creation
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array arr = nya_array_new(&arena, u32);
  nya_assert(arr.length == 0);
  nya_assert(arr.capacity == _NYA_ARRAY_DEFAULT_CAPACITY);
  nya_assert(arr.items != nullptr);
  nya_assert(arr.arena == &arena);
  nya_array_destroy(&arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: creation with custom capacity
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array arr_cap = nya_array_new_with_capacity(&arena, u32, 128);
  nya_assert(arr_cap.length == 0);
  nya_assert(arr_cap.capacity == 128);
  nya_array_destroy(&arr_cap);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_add (push_back)
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array add_arr = nya_array_new(&arena, u32);
  nya_array_add(&add_arr, 10U);
  nya_assert(add_arr.length == 1);
  nya_assert(add_arr.items[0] == 10);

  nya_array_add(&add_arr, 20U);
  nya_array_add(&add_arr, 30U);
  nya_assert(add_arr.length == 3);
  nya_assert(add_arr.items[0] == 10);
  nya_assert(add_arr.items[1] == 20);
  nya_assert(add_arr.items[2] == 30);
  nya_array_destroy(&add_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_add_many
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array add_many_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&add_many_arr, 1U, 2U, 3U, 4U, 5U);
  nya_assert(add_many_arr.length == 5);
  for (u32 i = 0; i < 5; ++i) { nya_assert(add_many_arr.items[i] == i + 1); }
  nya_array_destroy(&add_many_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: automatic resize on capacity overflow
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array resize_arr = nya_array_new_with_capacity(&arena, u32, 4);
  nya_assert(resize_arr.capacity == 4);
  for (u32 i = 0; i < 10; ++i) { nya_array_add(&resize_arr, i * 10); }
  nya_assert(resize_arr.length == 10);
  nya_assert(resize_arr.capacity >= 10);
  for (u32 i = 0; i < 10; ++i) { nya_assert(resize_arr.items[i] == i * 10); }
  nya_array_destroy(&resize_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_insert
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array insert_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&insert_arr, 10U, 30U, 40U);
  nya_array_insert(&insert_arr, 20U, 1);
  nya_assert(insert_arr.length == 4);
  nya_assert(insert_arr.items[0] == 10);
  nya_assert(insert_arr.items[1] == 20);
  nya_assert(insert_arr.items[2] == 30);
  nya_assert(insert_arr.items[3] == 40);
  nya_array_destroy(&insert_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_insert_many
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array insert_many_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&insert_many_arr, 1U, 5U);
  nya_array_insert_many(&insert_many_arr, 1, 2U, 3U, 4U);
  nya_assert(insert_many_arr.length == 5);
  for (u32 i = 0; i < 5; ++i) { nya_assert(insert_many_arr.items[i] == i + 1); }
  nya_array_destroy(&insert_many_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_remove
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array remove_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&remove_arr, 10U, 20U, 30U, 40U, 50U);
  u32 removed = nya_array_remove(&remove_arr, 2);
  nya_assert(removed == 30);
  nya_assert(remove_arr.length == 4);
  nya_assert(remove_arr.items[0] == 10);
  nya_assert(remove_arr.items[1] == 20);
  nya_assert(remove_arr.items[2] == 40);
  nya_assert(remove_arr.items[3] == 50);
  nya_array_destroy(&remove_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_remove at index 0
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array remove_first_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&remove_first_arr, 100U, 200U, 300U);
  u32 removed_first = nya_array_remove(&remove_first_arr, 0);
  nya_assert(removed_first == 100);
  nya_assert(remove_first_arr.length == 2);
  nya_assert(remove_first_arr.items[0] == 200);
  nya_assert(remove_first_arr.items[1] == 300);
  nya_array_destroy(&remove_first_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_remove_many
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array remove_many_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&remove_many_arr, 1U, 2U, 3U, 4U, 5U, 6U, 7U);
  nya_array_remove_many(&remove_many_arr, 2, 3);
  nya_assert(remove_many_arr.length == 4);
  nya_assert(remove_many_arr.items[0] == 1);
  nya_assert(remove_many_arr.items[1] == 2);
  nya_assert(remove_many_arr.items[2] == 6);
  nya_assert(remove_many_arr.items[3] == 7);
  nya_array_destroy(&remove_many_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_remove_many from start
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array remove_many_start_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&remove_many_start_arr, 1U, 2U, 3U, 4U, 5U);
  nya_array_remove_many(&remove_many_start_arr, 0, 2);
  nya_assert(remove_many_start_arr.length == 3);
  nya_assert(remove_many_start_arr.items[0] == 3);
  nya_assert(remove_many_start_arr.items[1] == 4);
  nya_assert(remove_many_start_arr.items[2] == 5);
  nya_array_destroy(&remove_many_start_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_remove_item
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array remove_item_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&remove_item_arr, 100U, 200U, 300U, 400U);
  nya_array_remove_item(&remove_item_arr, 200U);
  nya_assert(remove_item_arr.length == 3);
  nya_assert(remove_item_arr.items[0] == 100);
  nya_assert(remove_item_arr.items[1] == 300);
  nya_assert(remove_item_arr.items[2] == 400);
  nya_array_destroy(&remove_item_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_remove_item first element
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array remove_item_first_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&remove_item_first_arr, 10U, 20U, 30U);
  nya_array_remove_item(&remove_item_first_arr, 10U);
  nya_assert(remove_item_first_arr.length == 2);
  nya_assert(remove_item_first_arr.items[0] == 20);
  nya_assert(remove_item_first_arr.items[1] == 30);
  nya_array_destroy(&remove_item_first_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_push_back
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array push_arr = nya_array_new(&arena, u32);
  nya_array_push_back(&push_arr, 1U);
  nya_array_push_back(&push_arr, 2U);
  nya_array_push_back(&push_arr, 3U);
  nya_assert(push_arr.length == 3);
  nya_assert(push_arr.items[0] == 1);
  nya_assert(push_arr.items[1] == 2);
  nya_assert(push_arr.items[2] == 3);
  nya_array_destroy(&push_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_push_back_many
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array push_many_arr = nya_array_new(&arena, u32);
  nya_array_push_back_many(&push_many_arr, 1U, 2U, 3U, 4U, 5U);
  nya_assert(push_many_arr.length == 5);
  for (u32 i = 0; i < 5; ++i) { nya_assert(push_many_arr.items[i] == i + 1); }
  nya_array_destroy(&push_many_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_pop_back
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array pop_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&pop_arr, 1U, 2U, 3U, 4U, 5U);

  u32 back = nya_array_pop_back(&pop_arr);
  nya_assert(back == 5);
  nya_assert(pop_arr.length == 4);

  u32 back2 = nya_array_pop_back(&pop_arr);
  nya_assert(back2 == 4);
  nya_assert(pop_arr.length == 3);
  nya_assert(pop_arr.items[0] == 1);
  nya_assert(pop_arr.items[1] == 2);
  nya_assert(pop_arr.items[2] == 3);
  nya_array_destroy(&pop_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_pop_back_many
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array pop_many_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&pop_many_arr, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U);
  nya_array_pop_back_many(&pop_many_arr, 2);
  nya_assert(pop_many_arr.length == 6);
  nya_array_pop_back_many(&pop_many_arr, 2);
  nya_assert(pop_many_arr.length == 4);
  nya_assert(pop_many_arr.items[0] == 1);
  nya_assert(pop_many_arr.items[3] == 4);
  nya_array_destroy(&pop_many_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_push_front
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array push_front_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&push_front_arr, 2U, 3U, 4U);
  nya_array_push_front(&push_front_arr, 1U);
  nya_assert(push_front_arr.length == 4);
  nya_assert(push_front_arr.items[0] == 1);
  nya_assert(push_front_arr.items[1] == 2);
  nya_assert(push_front_arr.items[2] == 3);
  nya_assert(push_front_arr.items[3] == 4);
  nya_array_destroy(&push_front_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_push_front_many
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array push_front_many_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&push_front_many_arr, 4U, 5U);
  nya_array_push_front_many(&push_front_many_arr, 1U, 2U, 3U);
  nya_assert(push_front_many_arr.length == 5);
  for (u32 i = 0; i < 5; ++i) { nya_assert(push_front_many_arr.items[i] == i + 1); }
  nya_array_destroy(&push_front_many_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_pop_front
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array pop_front_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&pop_front_arr, 10U, 20U, 30U, 40U);
  u32 front = nya_array_pop_front(&pop_front_arr);
  nya_assert(front == 10);
  nya_assert(pop_front_arr.length == 3);
  nya_assert(pop_front_arr.items[0] == 20);
  nya_assert(pop_front_arr.items[1] == 30);
  nya_assert(pop_front_arr.items[2] == 40);
  nya_array_destroy(&pop_front_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_pop_front_many
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array pop_front_many_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&pop_front_many_arr, 1U, 2U, 3U, 4U, 5U, 6U);
  nya_array_pop_front_many(&pop_front_many_arr, 2);
  nya_assert(pop_front_many_arr.length == 4);
  nya_assert(pop_front_many_arr.items[0] == 3);
  nya_assert(pop_front_many_arr.items[3] == 6);
  nya_array_destroy(&pop_front_many_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_get / nya_array_set (including index 0)
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array access_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&access_arr, 10U, 20U, 30U, 40U);
  nya_assert(nya_array_get(&access_arr, 0) == 10);
  nya_assert(nya_array_get(&access_arr, 1) == 20);
  nya_assert(nya_array_get(&access_arr, 3) == 40);
  nya_array_set(&access_arr, 0, 999U);
  nya_assert(access_arr.items[0] == 999);
  nya_array_set(&access_arr, 2, 888U);
  nya_assert(access_arr.items[2] == 888);
  nya_array_destroy(&access_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_first and nya_array_last
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array fl_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&fl_arr, 100U, 200U, 300U);
  nya_assert(nya_array_first(&fl_arr) == 100);
  nya_assert(nya_array_last(&fl_arr) == 300);
  nya_array_destroy(&fl_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_contains
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array contains_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&contains_arr, 5U, 10U, 15U, 20U);
  nya_assert(nya_array_contains(&contains_arr, 10U) == true);
  nya_assert(nya_array_contains(&contains_arr, 20U) == true);
  nya_assert(nya_array_contains(&contains_arr, 25U) == false);
  nya_assert(nya_array_contains(&contains_arr, 0U) == false);
  nya_array_destroy(&contains_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_find
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array find_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&find_arr, 100U, 200U, 300U, 400U);
  nya_assert(nya_array_find(&find_arr, 100U) == 0);
  nya_assert(nya_array_find(&find_arr, 300U) == 2);
  nya_assert(nya_array_find(&find_arr, 400U) == 3);
  nya_assert(nya_array_find(&find_arr, 999U) == -1);
  nya_array_destroy(&find_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_equals
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array eq_arr1 = nya_array_new(&arena, u32);
  u32Array eq_arr2 = nya_array_new(&arena, u32);
  u32Array eq_arr3 = nya_array_new(&arena, u32);
  nya_array_add_many(&eq_arr1, 1U, 2U, 3U);
  nya_array_add_many(&eq_arr2, 1U, 2U, 3U);
  nya_array_add_many(&eq_arr3, 1U, 2U, 4U);
  nya_assert(nya_array_equals(&eq_arr1, &eq_arr2) == true);
  nya_assert(nya_array_equals(&eq_arr1, &eq_arr3) == false);

  u32Array eq_arr4 = nya_array_new(&arena, u32);
  nya_array_add_many(&eq_arr4, 1U, 2U);
  nya_assert(nya_array_equals(&eq_arr1, &eq_arr4) == false);

  nya_array_destroy(&eq_arr1);
  nya_array_destroy(&eq_arr2);
  nya_array_destroy(&eq_arr3);
  nya_array_destroy(&eq_arr4);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_swap (including index 0)
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array swap_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&swap_arr, 10U, 20U, 30U, 40U);
  nya_array_swap(&swap_arr, 0, 3);
  nya_assert(swap_arr.items[0] == 40);
  nya_assert(swap_arr.items[3] == 10);
  nya_array_swap(&swap_arr, 1, 2);
  nya_assert(swap_arr.items[1] == 30);
  nya_assert(swap_arr.items[2] == 20);
  nya_array_destroy(&swap_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_reverse
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array reverse_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&reverse_arr, 1U, 2U, 3U, 4U, 5U);
  nya_array_reverse(&reverse_arr);
  nya_assert(reverse_arr.items[0] == 5);
  nya_assert(reverse_arr.items[1] == 4);
  nya_assert(reverse_arr.items[2] == 3);
  nya_assert(reverse_arr.items[3] == 2);
  nya_assert(reverse_arr.items[4] == 1);
  nya_array_destroy(&reverse_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_reverse with even count
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array reverse_even_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&reverse_even_arr, 10U, 20U, 30U, 40U);
  nya_array_reverse(&reverse_even_arr);
  nya_assert(reverse_even_arr.items[0] == 40);
  nya_assert(reverse_even_arr.items[1] == 30);
  nya_assert(reverse_even_arr.items[2] == 20);
  nya_assert(reverse_even_arr.items[3] == 10);
  nya_array_destroy(&reverse_even_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_sort ascending
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array sort_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&sort_arr, 50U, 20U, 40U, 10U, 30U);
  nya_array_sort(&sort_arr, compare_u32_asc);
  nya_assert(sort_arr.items[0] == 10);
  nya_assert(sort_arr.items[1] == 20);
  nya_assert(sort_arr.items[2] == 30);
  nya_assert(sort_arr.items[3] == 40);
  nya_assert(sort_arr.items[4] == 50);
  nya_array_destroy(&sort_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_sort descending
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array sort_desc_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&sort_desc_arr, 30U, 10U, 50U, 20U, 40U);
  nya_array_sort(&sort_desc_arr, compare_u32_desc);
  nya_assert(sort_desc_arr.items[0] == 50);
  nya_assert(sort_desc_arr.items[1] == 40);
  nya_assert(sort_desc_arr.items[2] == 30);
  nya_assert(sort_desc_arr.items[3] == 20);
  nya_assert(sort_desc_arr.items[4] == 10);
  nya_array_destroy(&sort_desc_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_sort with signed integers
  // ─────────────────────────────────────────────────────────────────────────────
  s32Array sort_s32_arr = nya_array_new(&arena, s32);
  nya_array_add_many(&sort_s32_arr, 5, -10, 0, -5, 10);
  nya_array_sort(&sort_s32_arr, compare_s32_asc);
  nya_assert(sort_s32_arr.items[0] == -10);
  nya_assert(sort_s32_arr.items[1] == -5);
  nya_assert(sort_s32_arr.items[2] == 0);
  nya_assert(sort_s32_arr.items[3] == 5);
  nya_assert(sort_s32_arr.items[4] == 10);
  nya_array_destroy(&sort_s32_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_clear
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array clear_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&clear_arr, 1U, 2U, 3U, 4U, 5U);
  nya_assert(clear_arr.length == 5);
  u64 cap_before_clear = clear_arr.capacity;
  nya_array_clear(&clear_arr);
  nya_assert(clear_arr.length == 0);
  nya_assert(clear_arr.capacity == cap_before_clear);
  nya_array_add(&clear_arr, 999U);
  nya_assert(clear_arr.items[0] == 999);
  nya_array_destroy(&clear_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_resize
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array resize_manual_arr = nya_array_new_with_capacity(&arena, u32, 8);
  nya_array_add_many(&resize_manual_arr, 1U, 2U, 3U, 4U);
  nya_array_resize(&resize_manual_arr, 64);
  nya_assert(resize_manual_arr.capacity == 64);
  nya_assert(resize_manual_arr.length == 4);
  for (u32 i = 0; i < 4; ++i) { nya_assert(resize_manual_arr.items[i] == i + 1); }
  nya_array_destroy(&resize_manual_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_reserve
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array reserve_arr = nya_array_new_with_capacity(&arena, u32, 4);
  nya_array_add_many(&reserve_arr, 1U, 2U);
  nya_array_reserve(&reserve_arr, 100UL);
  nya_assert(reserve_arr.capacity >= 100);
  nya_assert(reserve_arr.length == 2);
  nya_assert(reserve_arr.items[0] == 1);
  nya_assert(reserve_arr.items[1] == 2);

  u64 cap = reserve_arr.capacity;
  nya_array_reserve(&reserve_arr, 50UL);
  nya_assert(reserve_arr.capacity == cap);
  nya_array_destroy(&reserve_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_extend
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array ext_arr1 = nya_array_new(&arena, u32);
  u32Array ext_arr2 = nya_array_new(&arena, u32);
  nya_array_add_many(&ext_arr1, 1U, 2U, 3U);
  nya_array_add_many(&ext_arr2, 4U, 5U, 6U);
  nya_array_extend(&ext_arr1, &ext_arr2);
  nya_assert(ext_arr1.length == 6);
  for (u32 i = 0; i < 6; ++i) { nya_assert(ext_arr1.items[i] == i + 1); }
  nya_array_destroy(&ext_arr1);
  nya_array_destroy(&ext_arr2);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_copy
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array clone_src = nya_array_new(&arena, u32);
  nya_array_add_many(&clone_src, 10U, 20U, 30U, 40U);
  u32Array clone_dst = nya_array_copy(&clone_src);
  nya_assert(clone_dst.length == clone_src.length);
  nya_assert(clone_dst.capacity == clone_src.capacity);
  nya_assert(clone_dst.items != clone_src.items);
  for (u64 i = 0; i < clone_src.length; ++i) { nya_assert(clone_dst.items[i] == clone_src.items[i]); }

  clone_dst.items[0] = 999;
  nya_assert(clone_src.items[0] == 10);
  nya_array_destroy(&clone_src);
  nya_array_destroy(&clone_dst);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_move
  // ─────────────────────────────────────────────────────────────────────────────
  NYA_Arena move_arena = nya_arena_new(.name = "move_arena");
  u32Array  move_arr   = nya_array_new(&arena, u32);
  nya_array_add_many(&move_arr, 100U, 200U, 300U);
  u32* old_items = move_arr.items;
  nya_array_move(&move_arr, &move_arena);
  nya_assert(move_arr.arena == &move_arena);
  nya_assert(move_arr.items != old_items);
  nya_assert(move_arr.items[0] == 100);
  nya_assert(move_arr.items[1] == 200);
  nya_assert(move_arr.items[2] == 300);
  nya_array_destroy(&move_arr);
  nya_arena_destroy(&move_arena);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_slice_excld
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array slice_src_excld = nya_array_new(&arena, u32);
  nya_array_add_many(&slice_src_excld, 10U, 20U, 30U, 40U, 50U);
  u32Array slice_excld = nya_array_slice_excld(&slice_src_excld, 1, 4);
  nya_assert(slice_excld.length == 3);
  nya_assert(slice_excld.items[0] == 20);
  nya_assert(slice_excld.items[1] == 30);
  nya_assert(slice_excld.items[2] == 40);
  nya_assert(slice_excld.arena == nullptr);
  nya_array_destroy(&slice_src_excld);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_slice_incld
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array slice_src_incld = nya_array_new(&arena, u32);
  nya_array_add_many(&slice_src_incld, 10U, 20U, 30U, 40U, 50U);
  u32Array slice_incld = nya_array_slice_incld(&slice_src_incld, 1, 3);
  nya_assert(slice_incld.length == 3);
  nya_assert(slice_incld.items[0] == 20);
  nya_assert(slice_incld.items[1] == 30);
  nya_assert(slice_incld.items[2] == 40);
  nya_assert(slice_incld.arena == nullptr);
  nya_array_destroy(&slice_src_incld);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_from_carray
  // ─────────────────────────────────────────────────────────────────────────────
  u32      carray[]    = {5, 10, 15, 20, 25};
  u32*     carray_ptr  = carray;
  u32Array from_carray = nya_array_from_carray(&arena, u32, carray_ptr, (u64)nya_carray_length(carray));
  nya_assert(from_carray.length == 5);
  for (u32 i = 0; i < 5; ++i) { nya_assert(from_carray.items[i] == (i + 1) * 5); }
  nya_array_destroy(&from_carray);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_length
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array len_arr = nya_array_new(&arena, u32);
  nya_assert(nya_array_length(&len_arr) == 0);
  nya_array_add_many(&len_arr, 1U, 2U, 3U);
  nya_assert(nya_array_length(&len_arr) == 3);
  nya_array_destroy(&len_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_for iterator
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array for_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&for_arr, 10U, 20U, 30U, 40U, 50U);
  u64 sum   = 0;
  u64 count = 0;
  nya_array_for (&for_arr, i) {
    sum += for_arr.items[i];
    count++;
  }
  nya_assert(sum == 150);
  nya_assert(count == 5);
  nya_array_destroy(&for_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_foreach iterator
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array foreach_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&foreach_arr, 1U, 2U, 3U, 4U, 5U);
  u64 foreach_sum = 0;
  nya_array_foreach (&foreach_arr, item) { foreach_sum += *item; }
  nya_assert(foreach_sum == 15);
  nya_array_destroy(&foreach_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_for_reverse iterator
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array for_rev_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&for_rev_arr, 1U, 2U, 3U, 4U, 5U);
  u32 collected[5];
  u64 collect_idx = 0;
  nya_array_for_reverse (&for_rev_arr, ri) { collected[collect_idx++] = for_rev_arr.items[ri]; }
  nya_assert(collected[0] == 5);
  nya_assert(collected[1] == 4);
  nya_assert(collected[2] == 3);
  nya_assert(collected[3] == 2);
  nya_assert(collected[4] == 1);
  nya_array_destroy(&for_rev_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_array_foreach_reverse iterator
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array foreach_rev_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&foreach_rev_arr, 10U, 20U, 30U);
  u32 foreach_collected[3];
  u64 foreach_idx = 0;
  nya_array_foreach_reverse (&foreach_rev_arr, item) { foreach_collected[foreach_idx++] = *item; }
  nya_assert(foreach_collected[0] == 30);
  nya_assert(foreach_collected[1] == 20);
  nya_assert(foreach_collected[2] == 10);
  nya_array_destroy(&foreach_rev_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: custom struct array
  // ─────────────────────────────────────────────────────────────────────────────
  TestPointArray points = nya_array_new(&arena, TestPoint);
  nya_array_add(&points, ((TestPoint){.x = 1, .y = 2}));
  nya_array_add(&points, ((TestPoint){.x = 3, .y = 4}));
  nya_array_add(&points, ((TestPoint){.x = 5, .y = 6}));
  nya_assert(points.length == 3);
  nya_assert(points.items[0].x == 1 && points.items[0].y == 2);
  nya_assert(points.items[1].x == 3 && points.items[1].y == 4);
  nya_assert(points.items[2].x == 5 && points.items[2].y == 6);
  nya_array_destroy(&points);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: s32 (signed) array
  // ─────────────────────────────────────────────────────────────────────────────
  s32Array s32_arr = nya_array_new(&arena, s32);
  nya_array_add_many(&s32_arr, -10, -5, 0, 5, 10);
  nya_assert(s32_arr.length == 5);
  nya_assert(s32_arr.items[0] == -10);
  nya_assert(s32_arr.items[2] == 0);
  nya_assert(s32_arr.items[4] == 10);
  nya_array_destroy(&s32_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: f32 (float) array
  // ─────────────────────────────────────────────────────────────────────────────
  f32Array f32_arr = nya_array_new(&arena, f32);
  nya_array_add_many(&f32_arr, 1.5F, 2.5F, 3.5F);
  nya_assert(f32_arr.length == 3);
  nya_assert(f32_arr.items[0] == 1.5F);
  nya_assert(f32_arr.items[1] == 2.5F);
  nya_assert(f32_arr.items[2] == 3.5F);
  nya_array_destroy(&f32_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: f64 (double) array
  // ─────────────────────────────────────────────────────────────────────────────
  f64Array f64_arr = nya_array_new(&arena, f64);
  nya_array_add_many(&f64_arr, 1.123456789, 2.234567890, 3.345678901);
  nya_assert(f64_arr.length == 3);
  nya_assert(f64_arr.items[0] == 1.123456789);
  nya_assert(f64_arr.items[1] == 2.234567890);
  nya_assert(f64_arr.items[2] == 3.345678901);
  nya_array_destroy(&f64_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: u8 (byte) array
  // ─────────────────────────────────────────────────────────────────────────────
  u8Array u8_arr = nya_array_new(&arena, u8);
  for (u32 i = 0; i < 256; ++i) { nya_array_add(&u8_arr, (u8)i); }
  nya_assert(u8_arr.length == 256);
  for (u32 i = 0; i < 256; ++i) { nya_assert(u8_arr.items[i] == (u8)i); }
  nya_array_destroy(&u8_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: u64 array
  // ─────────────────────────────────────────────────────────────────────────────
  u64Array u64_arr = nya_array_new(&arena, u64);
  nya_array_add(&u64_arr, (u64)0xFFFFFFFFFFFFFFFFULL);
  nya_array_add(&u64_arr, (u64)0);
  nya_array_add(&u64_arr, (u64)0x123456789ABCDEF0ULL);
  nya_assert(u64_arr.length == 3);
  nya_assert(u64_arr.items[0] == 0xFFFFFFFFFFFFFFFFULL);
  nya_assert(u64_arr.items[1] == 0);
  nya_assert(u64_arr.items[2] == 0x123456789ABCDEF0ULL);
  nya_array_destroy(&u64_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: pointer array (voidptr)
  // ─────────────────────────────────────────────────────────────────────────────
  voidptrArray ptr_arr = nya_array_new(&arena, voidptr);
  s32          val1    = 1;
  s32          val2    = 2;
  s32          val3    = 3;
  nya_array_add(&ptr_arr, (voidptr)&val1);
  nya_array_add(&ptr_arr, (voidptr)&val2);
  nya_array_add(&ptr_arr, (voidptr)&val3);
  nya_assert(ptr_arr.length == 3);
  nya_assert(*(s32*)ptr_arr.items[0] == 1);
  nya_assert(*(s32*)ptr_arr.items[1] == 2);
  nya_assert(*(s32*)ptr_arr.items[2] == 3);
  nya_array_destroy(&ptr_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: empty array operations
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array empty_arr = nya_array_new(&arena, u32);
  nya_assert(empty_arr.length == 0);
  nya_assert(nya_array_contains(&empty_arr, 42U) == false);
  nya_assert(nya_array_find(&empty_arr, 42U) == -1);
  nya_array_clear(&empty_arr);
  nya_assert(empty_arr.length == 0);
  nya_array_destroy(&empty_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: single element array
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array single_arr = nya_array_new(&arena, u32);
  nya_array_add(&single_arr, 42U);
  nya_assert(single_arr.length == 1);
  nya_assert(single_arr.items[0] == 42);
  nya_assert(nya_array_contains(&single_arr, 42U) == true);
  nya_assert(nya_array_find(&single_arr, 42U) == 0);
  nya_array_destroy(&single_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: large array (stress test)
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array large_arr = nya_array_new(&arena, u32);
  for (u32 i = 0; i < 10000; ++i) { nya_array_add(&large_arr, i); }
  nya_assert(large_arr.length == 10000);
  for (u32 i = 0; i < 10000; ++i) { nya_assert(large_arr.items[i] == i); }
  nya_array_destroy(&large_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: stress test with mixed operations
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_RNG             rng        = nya_rng_new();
    u32Array            stress_arr = nya_array_new(&arena, u32);
    NYA_RNGDistribution val_dist   = {
          .type    = NYA_RNG_DISTRIBUTION_UNIFORM,
          .uniform = {.min = 0, .max = 1000}
    };
    NYA_RNGDistribution op_dist = {
        .type    = NYA_RNG_DISTRIBUTION_UNIFORM,
        .uniform = {.min = 0, .max = 99}
    };

    for (u32 iter = 0; iter < 10000; ++iter) {
      u32 op = nya_rng_sample_u32(&rng, op_dist);

      if (op < 50) {
        u32 val = nya_rng_sample_u32(&rng, val_dist);
        nya_array_add(&stress_arr, val);
      } else if (op < 70 && stress_arr.length > 0) {
        nya_array_pop_back(&stress_arr);
      } else if (op < 85 && stress_arr.length > 0) {
        NYA_RNGDistribution idx_dist = {
            .type    = NYA_RNG_DISTRIBUTION_UNIFORM,
            .uniform = {.min = 0.0, .max = (f64)(stress_arr.length - 1)}
        };
        u64 idx = nya_rng_sample_u64(&rng, idx_dist);
        nya_array_remove(&stress_arr, idx);
      } else if (op < 95 && stress_arr.length > 0) {
        nya_array_pop_front(&stress_arr);
      } else {
        nya_array_clear(&stress_arr);
      }
    }
    nya_array_destroy(&stress_arr);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_carray_length
  // ─────────────────────────────────────────────────────────────────────────────
  s32 test_carray[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  nya_assert(nya_carray_length(test_carray) == 10);

  u8 byte_carray[] = {0, 1, 2};
  nya_assert(nya_carray_length(byte_carray) == 3);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: duplicate values in array
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array dup_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&dup_arr, 5U, 5U, 5U, 10U, 10U);
  nya_assert(dup_arr.length == 5);
  nya_assert(nya_array_contains(&dup_arr, 5U) == true);
  nya_assert(nya_array_find(&dup_arr, 5U) == 0);
  nya_assert(nya_array_find(&dup_arr, 10U) == 3);

  nya_array_remove_item(&dup_arr, 5U);
  nya_assert(dup_arr.length == 4);
  nya_assert(dup_arr.items[0] == 5);
  nya_array_destroy(&dup_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: slice modifications affect original
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array slice_mod_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&slice_mod_arr, 1U, 2U, 3U, 4U, 5U);
  u32Array slice_mod = nya_array_slice_incld(&slice_mod_arr, 1, 3);
  slice_mod.items[0] = 999;
  nya_assert(slice_mod_arr.items[1] == 999);
  nya_array_destroy(&slice_mod_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: multiple arrays in same arena
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array multi_arr1 = nya_array_new(&arena, u32);
  u32Array multi_arr2 = nya_array_new(&arena, u32);
  s32Array multi_arr3 = nya_array_new(&arena, s32);
  f32Array multi_arr4 = nya_array_new(&arena, f32);

  nya_array_add_many(&multi_arr1, 1U, 2U, 3U);
  nya_array_add_many(&multi_arr2, 10U, 20U, 30U);
  nya_array_add_many(&multi_arr3, -1, -2, -3);
  nya_array_add_many(&multi_arr4, 1.0F, 2.0F, 3.0F);

  nya_assert(multi_arr1.items[0] == 1);
  nya_assert(multi_arr2.items[0] == 10);
  nya_assert(multi_arr3.items[0] == -1);
  nya_assert(multi_arr4.items[0] == 1.0F);

  nya_array_destroy(&multi_arr1);
  nya_array_destroy(&multi_arr2);
  nya_array_destroy(&multi_arr3);
  nya_array_destroy(&multi_arr4);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: remove at boundaries
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array boundary_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&boundary_arr, 1U, 2U, 3U, 4U, 5U);

  u32 first_removed = nya_array_remove(&boundary_arr, 0);
  nya_assert(first_removed == 1);
  nya_assert(boundary_arr.length == 4);
  nya_assert(boundary_arr.items[0] == 2);

  u32 last_removed = nya_array_remove(&boundary_arr, boundary_arr.length - 1);
  nya_assert(last_removed == 5);
  nya_assert(boundary_arr.length == 3);
  nya_assert(boundary_arr.items[boundary_arr.length - 1] == 4);

  u32 mid_removed = nya_array_remove(&boundary_arr, 1);
  nya_assert(mid_removed == 3);
  nya_assert(boundary_arr.items[1] == 4);
  nya_array_destroy(&boundary_arr);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: extend empty array
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array ext_empty1 = nya_array_new(&arena, u32);
  u32Array ext_empty2 = nya_array_new(&arena, u32);
  nya_array_add_many(&ext_empty2, 1U, 2U, 3U);
  nya_array_extend(&ext_empty1, &ext_empty2);
  nya_assert(ext_empty1.length == 3);
  nya_assert(ext_empty1.items[0] == 1);
  nya_array_destroy(&ext_empty1);
  nya_array_destroy(&ext_empty2);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: extend with empty array
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array ext_with_empty1 = nya_array_new(&arena, u32);
  u32Array ext_with_empty2 = nya_array_new(&arena, u32);
  nya_array_add_many(&ext_with_empty1, 1U, 2U, 3U);
  nya_array_extend(&ext_with_empty1, &ext_with_empty2);
  nya_assert(ext_with_empty1.length == 3);
  nya_array_destroy(&ext_with_empty1);
  nya_array_destroy(&ext_with_empty2);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: equals with empty arrays
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array eq_empty1 = nya_array_new(&arena, u32);
  u32Array eq_empty2 = nya_array_new(&arena, u32);
  nya_assert(nya_array_equals(&eq_empty1, &eq_empty2) == true);
  nya_array_add(&eq_empty1, 1U);
  nya_assert(nya_array_equals(&eq_empty1, &eq_empty2) == false);
  nya_array_destroy(&eq_empty1);
  nya_array_destroy(&eq_empty2);

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: destroy resets array state
  // ─────────────────────────────────────────────────────────────────────────────
  u32Array destroy_arr = nya_array_new(&arena, u32);
  nya_array_add_many(&destroy_arr, 1U, 2U, 3U);
  nya_array_destroy(&destroy_arr);
  nya_assert(destroy_arr.items == nullptr);
  nya_assert(destroy_arr.length == 0);
  nya_assert(destroy_arr.capacity == 0);

  nya_arena_destroy(&arena);
  return 0;
}
