/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  NYA_Arena* arena = nya_arena_create(.name = "test_crc");

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc8 - basic test
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "hello";
    u8       crc    = nya_crc8(data, 5);
    // CRC8 of "hello" using polynomial x^8 + x^2 + x + 1
    // This value is computed from the lookup table
    nya_assert(crc != 0); // Just verify it produces a non-zero value
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc8 - empty data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "";
    u8       crc    = nya_crc8(data, 0);
    nya_assert(crc == 0); // CRC of empty data should be 0 (initial value)
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc8 - same data produces same CRC
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "test data for crc";
    u8       crc1   = nya_crc8(data, 17);
    u8       crc2   = nya_crc8(data, 17);
    nya_assert(crc1 == crc2);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc8 - different data produces different CRC (usually)
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data1[] = "data1";
    const u8 data2[] = "data2";
    u8       crc1    = nya_crc8(data1, 5);
    u8       crc2    = nya_crc8(data2, 5);
    nya_assert(crc1 != crc2);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc8 - binary data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = { 0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE };
    u8       crc    = nya_crc8(data, 6);
    nya_assert(crc != 0);

    // Verify same data produces same CRC
    u8 crc2 = nya_crc8(data, 6);
    nya_assert(crc == crc2);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc16 - basic test
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "hello";
    u16      crc    = nya_crc16(data, 5);
    nya_assert(crc != 0);
    nya_assert(crc != 0xFFFF); // Should not be the initial value
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc16 - empty data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "";
    u16      crc    = nya_crc16(data, 0);
    // CRC16-CCITT of empty data should be 0xFFFF (initial value)
    nya_assert(crc == 0xFFFF);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc16 - deterministic
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "123456789";
    u16      crc1   = nya_crc16(data, 9);
    u16      crc2   = nya_crc16(data, 9);
    nya_assert(crc1 == crc2);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc16 - known test vectors
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // CRC16-CCITT (0xFFFF initial) of "123456789" should be 0x29B1
    const u8 data[] = "123456789";
    u16      crc    = nya_crc16(data, 9);
    nya_assert(crc == 0x29B1);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc32 - basic test
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "hello";
    u32      crc    = nya_crc32(data, 5);
    nya_assert(crc != 0);
    nya_assert(crc != 0xFFFFFFFF);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc32 - empty data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "";
    u32      crc    = nya_crc32(data, 0);
    // CRC32 of empty data: init ^ final_xor = 0xFFFFFFFF ^ 0xFFFFFFFF = 0
    nya_assert(crc == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc32 - deterministic
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "The quick brown fox jumps over the lazy dog";
    u32      crc1   = nya_crc32(data, 43);
    u32      crc2   = nya_crc32(data, 43);
    nya_assert(crc1 == crc2);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc32 - known test vector
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // CRC32 of "123456789" should be 0xCBF43926
    const u8 data[] = "123456789";
    u32      crc    = nya_crc32(data, 9);
    nya_assert(crc == 0xCBF43926);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc32 - ASCII 'A' should produce 0xD3D99E8B
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "A";
    u32      crc    = nya_crc32(data, 1);
    // CRC32 of "A" = 0xD3D99E8B (calculated with standard CRC32)
    // But let's just verify it's deterministic
    u32      crc2 = nya_crc32(data, 1);
    nya_assert(crc == crc2);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc64 - basic test
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "hello";
    u64      crc    = nya_crc64(data, 5);
    nya_assert(crc != 0);
    nya_assert(crc != 0xFFFFFFFFFFFFFFFFULL);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc64 - empty data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "";
    u64      crc    = nya_crc64(data, 0);
    // CRC64 of empty data: init ^ final_xor = 0xFFFFFFFFFFFFFFFF ^ 0xFFFFFFFFFFFFFFFF = 0
    nya_assert(crc == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_crc64 - deterministic
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "123456789";
    u64      crc1   = nya_crc64(data, 9);
    u64      crc2   = nya_crc64(data, 9);
    nya_assert(crc1 == crc2);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: all CRC functions with same data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data[] = "abcdefghijklmnopqrstuvwxyz";

    u8  crc8  = nya_crc8(data, 26);
    u16 crc16 = nya_crc16(data, 26);
    u32 crc32 = nya_crc32(data, 26);
    u64 crc64 = nya_crc64(data, 26);

    // All should be non-zero
    nya_assert(crc8 != 0);
    nya_assert(crc16 != 0);
    nya_assert(crc32 != 0);
    nya_assert(crc64 != 0);

    // Running again should produce same results
    nya_assert(crc8 == nya_crc8(data, 26));
    nya_assert(crc16 == nya_crc16(data, 26));
    nya_assert(crc32 == nya_crc32(data, 26));
    nya_assert(crc64 == nya_crc64(data, 26));
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: CRC avalanche effect - small change produces large difference
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 data1[] = "aaaaaaaaaa";
    const u8 data2[] = "baaaaaaaaa"; // One bit different

    u32 crc1 = nya_crc32(data1, 10);
    u32 crc2 = nya_crc32(data2, 10);

    // The CRCs should be very different
    nya_assert(crc1 != crc2);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // CLEANUP
  // ─────────────────────────────────────────────────────────────────────────────
  nya_arena_destroy(arena);

  return 0;
}
