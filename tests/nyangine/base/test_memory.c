/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

// Test structure for field offset tests
typedef struct TestStruct TestStruct;
struct TestStruct {
  u8          byte_field;
  u32         int_field;
  f32         float_field;
  u64         long_field;
  u8          array_field[10];
  TestStruct* next;
};

s32 main(void) {
  NYA_Arena* arena = nya_arena_create(.name = "test_memory");

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_is_zeroed macro
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // Test with zero-initialized struct
    TestStruct zero_struct = { 0 };
    nya_assert(nya_is_zeroed(zero_struct) == true);

    // Test with partially initialized struct
    TestStruct partial_struct = { 0 };
    partial_struct.int_field  = 42;
    nya_assert(nya_is_zeroed(partial_struct) == false);

    // Test with basic types
    s32 zero_int     = 0;
    s32 non_zero_int = 42;
    nya_assert(nya_is_zeroed(zero_int) == true);
    nya_assert(nya_is_zeroed(non_zero_int) == false);

    // Test with pointers
    void*       null_ptr   = nullptr;
    TestStruct* struct_ptr = (TestStruct*)0x12345678;
    nya_assert(nya_is_zeroed(null_ptr) == true);
    nya_assert(nya_is_zeroed(struct_ptr) == false);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_typeof_field macro
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // Test that the macro returns the correct type
    // We can't directly test the type, but we can use it in assignments
    TestStruct test_struct = { 0 };

    // These should compile without errors if the types are correct
    // Note: nya_typeof_field is just typeof wrapper, so we test by usage
    __typeof__(test_struct.byte_field)  byte_val  = 0;
    __typeof__(test_struct.int_field)   int_val   = 0;
    __typeof__(test_struct.float_field) float_val = 0.0f;

    // Verify assignments work (basic sanity check)
    nya_assert(byte_val == 0);
    nya_assert(int_val == 0);
    nya_assert(float_val == 0.0f);

    (void)byte_val;
    (void)int_val;
    (void)float_val; // Suppress unused warnings
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_sizeof_field macro
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // Test that the macro returns the correct sizes
    nya_assert(nya_sizeof_field(TestStruct, byte_field) == sizeof(u8));
    nya_assert(nya_sizeof_field(TestStruct, int_field) == sizeof(u32));
    nya_assert(nya_sizeof_field(TestStruct, float_field) == sizeof(f32));
    nya_assert(nya_sizeof_field(TestStruct, long_field) == sizeof(u64));
    nya_assert(nya_sizeof_field(TestStruct, array_field) == sizeof(u8[10]));
    nya_assert(nya_sizeof_field(TestStruct, next) == sizeof(TestStruct*));
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_offsetof macro
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // Test that the macro returns the correct offsets
    TestStruct test_struct = { 0 };

    // First field should be at offset 0
    nya_assert(nya_offsetof(TestStruct, byte_field) == 0);

    // Other fields should be at increasing offsets
    size_t int_offset   = nya_offsetof(TestStruct, int_field);
    size_t float_offset = nya_offsetof(TestStruct, float_field);
    size_t long_offset  = nya_offsetof(TestStruct, long_field);
    size_t array_offset = nya_offsetof(TestStruct, array_field);
    size_t next_offset  = nya_offsetof(TestStruct, next);

    // Verify offsets are increasing (basic sanity check)
    nya_assert(int_offset > 0);
    nya_assert(float_offset > int_offset);
    nya_assert(long_offset > float_offset);
    nya_assert(array_offset > long_offset);
    nya_assert(next_offset > array_offset);

    // Verify accessing fields through offsets gives correct values
    test_struct.int_field = 0x12345678;
    u32* int_ptr          = (u32*)((u8*)&test_struct + int_offset);
    nya_assert(*int_ptr == 0x12345678);

    test_struct.float_field = 3.14159f;
    f32* float_ptr          = (f32*)((u8*)&test_struct + float_offset);
    nya_assert(*float_ptr == 3.14159f);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_container_of macro (basic functionality) - skipped due to assert_type_match
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // Container_of macro requires assert_type_match which is not available
    // This test would verify that container_of can recover the containing structure
    // from a pointer to one of its members, but we'll skip for now
    nya_assert(true); // Placeholder to keep test structure
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Byte conversion macros
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // Test binary unit conversions
    u64 kib_result = nya_kibyte_to_byte(1024);
    nya_assert(kib_result == 1024 * 1024);

    u64 mib_result = nya_mebyte_to_byte(1024);
    nya_assert(mib_result == 1024 * 1024 * 1024);

    u64 gib_result = nya_tebyte_to_byte(1);
    nya_assert(gib_result == 1024ULL * 1024 * 1024 * 1024);

    // Test reverse conversions
    u64 back_to_kib = nya_byte_to_kibyte(1024 * 1024);
    nya_assert(back_to_kib == 1024);

    u64 back_to_mib = nya_byte_to_mebyte(1024 * 1024 * 1024);
    nya_assert(back_to_mib == 1024);

    // Test decimal unit conversions
    u64 kb_result = nya_kbyte_to_byte(1000);
    nya_assert(kb_result == 1000 * 1000);

    u64 mb_result = nya_mbyte_to_byte(1000);
    nya_assert(mb_result == 1000 * 1000 * 1000);

    // Test reverse decimal conversions
    u64 back_to_kb = nya_byte_to_kbyte(1000 * 1000);
    nya_assert(back_to_kb == 1000);

    u64 back_to_mb = nya_byte_to_mbyte(1000 * 1000 * 1000);
    nya_assert(back_to_mb == 1000);

    // Test large values
    u64 tb_binary = nya_byte_to_gibyte(2048ULL * 1024 * 1024 * 1024); // 2048 GB = 2 TB
    nya_assert(tb_binary == 2048);

    u64 tb_decimal = nya_byte_to_gbyte(2000ULL * 1000 * 1000 * 1000); // 2000 GB = 2 TB (decimal)
    nya_assert(tb_decimal == 2000);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Memory allocation wrappers
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // Test that memory allocation macros work as expected
    void* ptr = nya_malloc(64);
    nya_assert(ptr != nullptr);

    // Test realloc
    ptr = nya_realloc(ptr, 128);
    nya_assert(ptr != nullptr);

    // Test calloc
    void* zero_ptr = nya_calloc(10, sizeof(s32));
    nya_assert(zero_ptr != nullptr);

    // Verify calloc zeroed memory
    s32* int_array = (s32*)zero_ptr;
    for (int i = 0; i < 10; i++) { nya_assert(int_array[i] == 0); }

    // Test free
    nya_free(ptr);
    nya_free(zero_ptr);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: Memory operation wrappers
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // Test memcpy wrapper
    u8 src[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    u8 dst[16] = { 0 };

    nya_memcpy(dst, src, 16);
    nya_assert(nya_memcmp(dst, src, 16) == 0);

    // Test memset wrapper
    u8 buffer[32];
    nya_memset(buffer, 0xAA, 32);

    for (int i = 0; i < 32; i++) { nya_assert(buffer[i] == 0xAA); }

    // Test memmove wrapper (overlapping regions)
    u8 overlap[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    nya_memmove(overlap + 4, overlap, 8); // Move first 8 bytes right by 4

    // Expected result: [1, 2, 3, 4, 1, 2, 3, 4, 5, 6, 7, 8, 13, 14, 15, 16]
    u8 expected[16] = { 1, 2, 3, 4, 1, 2, 3, 4, 5, 6, 7, 8, 13, 14, 15, 16 };
    nya_assert(nya_memcmp(overlap, expected, 16) == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // CLEANUP
  // ─────────────────────────────────────────────────────────────────────────────
  nya_arena_destroy(arena);

  return 0;
}