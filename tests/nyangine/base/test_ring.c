/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

typedef struct {
  u32 id;
} TestItem;

nya_derive_ring(TestItem);

void test_ring_basic_operations(void) {
  NYA_Arena* arena = nya_arena_create();

  // Test: nya_ring_create and nya_ring_create_with_capacity
  TestItemRing* ring = nya_ring_create(arena, TestItem);
  nya_assert(ring != nullptr);
  nya_assert(nya_ring_capacity(ring) == 8);
  nya_assert(nya_ring_length(ring) == 0);
  nya_assert(nya_ring_is_empty(ring));

  // Test: nya_ring_push and basic access
  TestItem item1 = { .id = 1 };
  TestItem item2 = { .id = 2 };

  nya_ring_push(ring, item1);
  nya_assert(!nya_ring_is_empty(ring));
  nya_assert(nya_ring_length(ring) == 1);
  nya_assert(nya_ring_front(ring)->id == 1);
  nya_assert(nya_ring_back(ring)->id == 1);

  nya_ring_push(ring, item2);
  nya_assert(nya_ring_length(ring) == 2);
  nya_assert(nya_ring_front(ring)->id == 1);
  nya_assert(nya_ring_back(ring)->id == 2);

  // Test: nya_ring_at
  nya_assert(nya_ring_at(ring, 0)->id == 1);
  nya_assert(nya_ring_at(ring, 1)->id == 2);

  // Test: nya_ring_pop
  TestItem popped = nya_ring_pop(ring);
  nya_assert(popped.id == 1);
  nya_assert(nya_ring_length(ring) == 1);

  // Test: nya_ring_clear
  nya_ring_clear(ring);
  nya_assert(nya_ring_length(ring) == 0);
  nya_assert(nya_ring_is_empty(ring));

  nya_ring_destroy(ring);
  nya_arena_destroy(arena);
}

void test_ring_wraparound(void) {
  NYA_Arena* arena = nya_arena_create();

  // Create a small ring to test wraparound behavior
  TestItemRing* ring = nya_ring_create_with_capacity(arena, TestItem, 3);

  // Fill ring to capacity
  for (u32 i = 0; i < 3; i++) {
    TestItem item = { .id = i };
    nya_ring_push(ring, item);
  }

  // Push another item - should wrap around and overwrite the first
  TestItem wrap_item = { .id = 99 };
  nya_ring_push(ring, wrap_item);

  nya_assert(nya_ring_length(ring) == 3);
  nya_assert(nya_ring_capacity(ring) == 3);
  nya_assert(nya_ring_front(ring)->id == 1); // First item (id=0) was overwritten
  nya_assert(nya_ring_back(ring)->id == 99); // New item is at back

  // Pop items to verify correct order
  TestItem popped1 = nya_ring_pop(ring);
  nya_assert(popped1.id == 1);

  TestItem popped2 = nya_ring_pop(ring);
  nya_assert(popped2.id == 2);

  TestItem popped3 = nya_ring_pop(ring);
  nya_assert(popped3.id == 99);

  nya_assert(nya_ring_is_empty(ring));

  nya_ring_destroy(ring);
  nya_arena_destroy(arena);
}

void test_ring_iterators(void) {
  NYA_Arena* arena = nya_arena_create();

  TestItemRing* ring = nya_ring_create(arena, TestItem);

  // Add some items
  TestItem items[] = { { .id = 1 }, { .id = 2 }, { .id = 3 } };
  nya_ring_push_many(ring, items[0], items[1], items[2]);

  // Test: nya_ring_foreach
  u32       foreach_count = 0;
  TestItem* current_item;
  nya_ring_foreach(ring, current_item) {
    nya_assert(current_item->id == items[foreach_count].id);
    foreach_count++;
  }
  nya_assert(foreach_count == 3);

  // Test: nya_ring_foreach_reverse
  u32       reverse_count = 0;
  TestItem* reverse_item;
  nya_ring_foreach_reverse(ring, reverse_item) {
    nya_assert(reverse_item->id == items[2 - reverse_count].id);
    reverse_count++;
  }
  nya_assert(reverse_count == 3);

  nya_ring_destroy(ring);
  nya_arena_destroy(arena);
}

s32 main(void) {
  test_ring_basic_operations();
  test_ring_wraparound();
  test_ring_iterators();

  return 0;
}
