#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_guard.h"
#include "nyangine/base/base_memory.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef enum NYA_MemoryActionType    NYA_MemoryActionType;
typedef struct NYA_Arena             NYA_Arena;
typedef struct NYA_ArenaFreeList     NYA_ArenaFreeList;
typedef struct NYA_ArenaFreeListNode NYA_ArenaFreeListNode;
typedef struct NYA_ArenaOptions      NYA_ArenaOptions;
typedef struct NYA_ArenaRegion       NYA_ArenaRegion;
typedef struct NYA_MemoryAction      NYA_MemoryAction;
typedef struct NYA_MemoryActionArray NYA_MemoryActionArray;

#define _NYA_ARENA_DEFAULT_OPTIONS                                                                                                                   \
  .name = nullptr, .alignment = 8, .region_size = nya_gibyte_to_byte(1UL), .defragmentation_enabled = true, .defragmentation_threshold = 16,         \
  .garbage_collection_enabled = true, .garbage_collection_threshold = 3

struct NYA_ArenaOptions {
  const char* name;

  u8 alignment;

  u64 region_size;

  b8 defragmentation_enabled;
  u8 defragmentation_threshold; /** number of frees before the free list is defragmented */

  /**
   * Every free_all will increase a counter for unused regions.
   * When the counter reaches this value, the region will be freed.
   * Calling nya_arena_garbage_collect will free all unused regions regardless of the counter
   * */
  b8 garbage_collection_enabled;
  u8 garbage_collection_threshold;
};

struct NYA_Arena {
  NYA_ArenaOptions options;
  NYA_ArenaRegion *head, *tail;
};

struct NYA_ArenaRegion {
  u64 used;
  u64 capacity;
  u8* memory;

  u8                 gc_counter;
  NYA_ArenaFreeList* free_list;

  NYA_ArenaRegion *next, *prev;
};

struct NYA_ArenaFreeList {
  u32 node_counter;
  f32 average_free_size;
  u8  defragmentation_counter;

  NYA_ArenaFreeListNode *head, *tail;
};

struct NYA_ArenaFreeListNode {
  void* ptr;
  u64   size;

  NYA_ArenaFreeListNode *prev, *next;
};

enum NYA_MemoryActionType {
  NYA_MEMORY_ACTION_ARENA_NEW,
  NYA_MEMORY_ACTION_ALLOC,
  NYA_MEMORY_ACTION_REALLOC,
  NYA_MEMORY_ACTION_FREE,
  NYA_MEMORY_ACTION_FREE_ALL,
  NYA_MEMORY_ACTION_GARBAGE_COLLECT,
  NYA_MEMORY_ACTION_ARENA_DESTROY,
  NYA_MEMORY_ACTION_COPY,
  NYA_MEMORY_ACTION_MOVE,
  NYA_MEMORY_ACTION_COUNT,
};

struct NYA_MemoryAction {
  NYA_MemoryActionType type;

  const char* arena_name;
  const char* file_name;
  u32         line_number;
  const char* function_name;

  union {
    struct {
      u8* ptr;
      u64 size;
    } alloc, free;

    struct {
      u8* old_ptr;
      u64 old_size;
      u8* new_ptr;
      u64 new_size;
    } realloc;

    struct {
      u8* ptr;
      u64 size;
      u8* copy_ptr;
    } copy;

    struct {
      u8*         ptr;
      u64         size;
      const char* target_arena_name;
      u8*         target_ptr;
    } move;
  };
};

/** not compatiple with base_array.h ! */
struct NYA_MemoryActionArray {
  u64               length;
  u64               capacity;
  NYA_MemoryAction* items;
};

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS AND MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN NYA_Arena nya_arena_global;
NYA_API NYA_EXTERN NYA_Arena nya_arena_temp;

#if (NYA_IS_DEBUG || defined(NYA_ARENA_FORCE_DEBUG)) && !defined(NYA_ARENA_FORCE_NODEBUG)
#define nya_arena_create(...)                                                                                                                        \
  _nya_arena_debug_create_with_options((NYA_ArenaOptions){ _NYA_ARENA_DEFAULT_OPTIONS, __VA_ARGS__ }, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_create_with_options(options)            _nya_arena_debug_create_with_options(options, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_alloc(arena, size)                      _nya_arena_debug_alloc(arena, size, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_realloc(arena, ptr, old_size, new_size) _nya_arena_debug_realloc(arena, ptr, old_size, new_size, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_free(arena, ptr, size)                  _nya_arena_debug_free(arena, ptr, size, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_free_all(arena)                         _nya_arena_debug_free_all(arena, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_garbage_collect(arena)                  _nya_arena_debug_garbage_collect(arena, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_destroy(arena)                          _nya_arena_debug_destroy(arena, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_copy(dst, ptr, size)                    _nya_arena_debug_copy(dst, ptr, size, __FUNCTION__, __FILE__, __LINE__)
#define nya_arena_move(src, dst, ptr, size)               _nya_arena_debug_move(src, dst, ptr, size, __FUNCTION__, __FILE__, __LINE__)
#else
#define nya_arena_create(...)         _nya_arena_nodebug_create_with_options((NYA_ArenaOptions){ _NYA_ARENA_DEFAULT_OPTIONS, __VA_ARGS__ })
#define nya_arena_create_with_options _nya_arena_nodebug_create_with_options
#define nya_arena_alloc               _nya_arena_nodebug_alloc
#define nya_arena_realloc             _nya_arena_nodebug_realloc
#define nya_arena_free                _nya_arena_nodebug_free
#define nya_arena_free_all            _nya_arena_nodebug_free_all
#define nya_arena_garbage_collect     _nya_arena_nodebug_garbage_collect
#define nya_arena_destroy             _nya_arena_nodebug_destroy
#define nya_arena_copy                _nya_arena_nodebug_copy
#define nya_arena_move                _nya_arena_nodebug_move
#endif // (NYA_IS_DEBUG || defined(NYA_ARENA_FORCE_DEBUG)) && !defined(NYA_ARENA_FORCE_NODEBUG)

NYA_API NYA_EXTERN NYA_MemoryActionArray* nya_arena_get_memory_actions(void) __attr_no_discard;
NYA_API NYA_EXTERN u64                    nya_arena_memory_usage(NYA_Arena* arena) __attr_no_discard;
NYA_API NYA_EXTERN void                   nya_arena_print(NYA_Arena* arena);

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * INTERNALS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

// clang-format off
NYA_API NYA_EXTERN NYA_Arena _nya_arena_debug_create_with_options(NYA_ArenaOptions options, const char* function, const char* file, u32 line) __attr_no_discard;
NYA_API NYA_EXTERN void*     _nya_arena_debug_alloc(NYA_Arena* arena, u64 size, const char* function, const char* file, u32 line) __attr_malloc __attr_no_discard;
NYA_API NYA_EXTERN void*     _nya_arena_debug_realloc(NYA_Arena* arena, void* ptr, u64 old_size, u64 new_size, const char* function, const char* file, u32 line) __attr_no_discard;
NYA_API NYA_EXTERN void      _nya_arena_debug_free(NYA_Arena* arena, void* ptr, u64 size, const char* function, const char* file, u32 line);
NYA_API NYA_EXTERN void      _nya_arena_debug_free_all(NYA_Arena* arena, const char* function, const char* file, u32 line);
NYA_API NYA_EXTERN void      _nya_arena_debug_garbage_collect(NYA_Arena* arena, const char* function, const char* file, u32 line);
NYA_API NYA_EXTERN void      _nya_arena_debug_destroy(NYA_Arena* arena, const char* function, const char* file, u32 line);
NYA_API NYA_EXTERN void*     _nya_arena_debug_copy(NYA_Arena* dst, void* ptr, u64 size, const char* function, const char* file, u32 line) __attr_no_discard;
NYA_API NYA_EXTERN void*     _nya_arena_debug_move(NYA_Arena* src, NYA_Arena* dst, void* ptr, u64 size, const char* function, const char* file, u32 line) __attr_no_discard;
// clang-format on

NYA_API NYA_EXTERN NYA_Arena _nya_arena_nodebug_create_with_options(NYA_ArenaOptions options) __attr_no_discard;
NYA_API NYA_EXTERN void*     _nya_arena_nodebug_alloc(NYA_Arena* arena, u64 size) __attr_malloc __attr_no_discard;
NYA_API NYA_EXTERN void*     _nya_arena_nodebug_realloc(NYA_Arena* arena, void* ptr, u64 old_size, u64 new_size) __attr_no_discard;
NYA_API NYA_EXTERN void      _nya_arena_nodebug_free(NYA_Arena* arena, void* ptr, u64 size);
NYA_API NYA_EXTERN void      _nya_arena_nodebug_free_all(NYA_Arena* arena);
NYA_API NYA_EXTERN void      _nya_arena_nodebug_garbage_collect(NYA_Arena* arena);
NYA_API NYA_EXTERN void      _nya_arena_nodebug_destroy(NYA_Arena* arena);
NYA_API NYA_EXTERN void*     _nya_arena_nodebug_copy(NYA_Arena* dst, void* ptr, u64 size) __attr_no_discard;
NYA_API NYA_EXTERN void*     _nya_arena_nodebug_move(NYA_Arena* src, NYA_Arena* dst, void* ptr, u64 size) __attr_no_discard;

NYA_DEFINE_CLEANUP_FN(nya_arena_destroy, NYA_Arena, arena, nya_arena_destroy(&arena))
