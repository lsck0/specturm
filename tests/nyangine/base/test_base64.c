/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  NYA_Arena* arena = nya_arena_create(.name = "test_base64");

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_base64_encode - basic encoding
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_String encoded = *nya_string_create(arena);
    const u8   data[]  = "hello";
    nya_base64_encode(&encoded, data, 5);
    nya_assert(encoded.length == 8);
    nya_assert(nya_string_equals(&encoded, "aGVsbG8=") == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_base64_encode - empty data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_String encoded = *nya_string_create(arena);
    const u8   data[]  = "";
    nya_base64_encode(&encoded, data, 0);
    nya_assert(encoded.length == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_base64_encode - various lengths (padding cases)
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // 1 byte -> 2 chars + 2 padding
    NYA_String encoded1 = *nya_string_create(arena);
    const u8   data1[]  = "f";
    nya_base64_encode(&encoded1, data1, 1);
    nya_assert(encoded1.length == 4);
    nya_assert(nya_string_equals(&encoded1, "Zg==") == true);

    // 2 bytes -> 3 chars + 1 padding
    NYA_String encoded2 = *nya_string_create(arena);
    const u8   data2[]  = "fo";
    nya_base64_encode(&encoded2, data2, 2);
    nya_assert(encoded2.length == 4);
    nya_assert(nya_string_equals(&encoded2, "Zm8=") == true);

    // 3 bytes -> 4 chars, no padding
    NYA_String encoded3 = *nya_string_create(arena);
    const u8   data3[]  = "foo";
    nya_base64_encode(&encoded3, data3, 3);
    nya_assert(encoded3.length == 4);
    nya_assert(nya_string_equals(&encoded3, "Zm9v") == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_base64_encode - longer strings
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_String encoded = *nya_string_create(arena);
    const u8   data[]  = "hello world";
    nya_base64_encode(&encoded, data, 11);
    nya_assert(encoded.length == 16);
    nya_assert(nya_string_equals(&encoded, "aGVsbG8gd29ybGQ=") == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_base64_encode - binary data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_String encoded = *nya_string_create(arena);
    const u8   data[]  = { 0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE };
    nya_base64_encode(&encoded, data, 6);
    nya_assert(encoded.length == 8);
    nya_assert(nya_string_equals(&encoded, "AAECA//+") == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_base64_decode - basic decoding
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_String decoded = *nya_string_create(arena);
    const u8   data[]  = "aGVsbG8=";
    nya_base64_decode(&decoded, data, 8);
    nya_assert(decoded.length == 5);
    nya_assert(nya_memcmp(decoded.items, "hello", 5) == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_base64_decode - empty data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_String decoded = *nya_string_create(arena);
    const u8   data[]  = "";
    nya_base64_decode(&decoded, data, 0);
    nya_assert(decoded.length == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_base64_decode - various padding cases
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // 2 padding chars
    NYA_String decoded1 = *nya_string_create(arena);
    const u8   data1[]  = "Zg==";
    nya_base64_decode(&decoded1, data1, 4);
    nya_assert(decoded1.length == 1);
    nya_assert(nya_memcmp(decoded1.items, "f", 1) == 0);

    // 1 padding char
    NYA_String decoded2 = *nya_string_create(arena);
    const u8   data2[]  = "Zm8=";
    nya_base64_decode(&decoded2, data2, 4);
    nya_assert(decoded2.length == 2);
    nya_assert(nya_memcmp(decoded2.items, "fo", 2) == 0);

    // no padding
    NYA_String decoded3 = *nya_string_create(arena);
    const u8   data3[]  = "Zm9v";
    nya_base64_decode(&decoded3, data3, 4);
    nya_assert(decoded3.length == 3);
    nya_assert(nya_memcmp(decoded3.items, "foo", 3) == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: nya_base64_decode - longer strings
  // ─────────────────────────────────────────────────────────────────────────────
  {
    NYA_String decoded = *nya_string_create(arena);
    const u8   data[]  = "aGVsbG8gd29ybGQ=";
    nya_base64_decode(&decoded, data, 16);
    nya_assert(decoded.length == 11);
    nya_assert(nya_memcmp(decoded.items, "hello world", 11) == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: round-trip encode -> decode
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 original[] = "The quick brown fox jumps over the lazy dog!";

    NYA_String encoded = *nya_string_create(arena);
    nya_base64_encode(&encoded, original, 44);

    NYA_String decoded = *nya_string_create(arena);
    nya_base64_decode(&decoded, encoded.items, encoded.length);

    nya_assert(decoded.length == 44);
    nya_assert(nya_memcmp(decoded.items, original, 44) == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: round-trip with binary data
  // ─────────────────────────────────────────────────────────────────────────────
  {
    const u8 original[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

    NYA_String encoded = *nya_string_create(arena);
    nya_base64_encode(&encoded, original, 16);

    NYA_String decoded = *nya_string_create(arena);
    nya_base64_decode(&decoded, encoded.items, encoded.length);

    nya_assert(decoded.length == 16);
    nya_assert(nya_memcmp(decoded.items, original, 16) == 0);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // TEST: known base64 test vectors
  // ─────────────────────────────────────────────────────────────────────────────
  {
    // "f" -> "Zg=="
    NYA_String enc1 = *nya_string_create(arena);
    nya_base64_encode(&enc1, (const u8*)"f", 1);
    nya_assert(nya_string_equals(&enc1, "Zg==") == true);

    // "fo" -> "Zm8="
    NYA_String enc2 = *nya_string_create(arena);
    nya_base64_encode(&enc2, (const u8*)"fo", 2);
    nya_assert(nya_string_equals(&enc2, "Zm8=") == true);

    // "foo" -> "Zm9v"
    NYA_String enc3 = *nya_string_create(arena);
    nya_base64_encode(&enc3, (const u8*)"foo", 3);
    nya_assert(nya_string_equals(&enc3, "Zm9v") == true);

    // "foob" -> "Zm9vYg=="
    NYA_String enc4 = *nya_string_create(arena);
    nya_base64_encode(&enc4, (const u8*)"foob", 4);
    nya_assert(nya_string_equals(&enc4, "Zm9vYg==") == true);

    // "fooba" -> "Zm9vYmE="
    NYA_String enc5 = *nya_string_create(arena);
    nya_base64_encode(&enc5, (const u8*)"fooba", 5);
    nya_assert(nya_string_equals(&enc5, "Zm9vYmE=") == true);

    // "foobar" -> "Zm9vYmFy"
    NYA_String enc6 = *nya_string_create(arena);
    nya_base64_encode(&enc6, (const u8*)"foobar", 6);
    nya_assert(nya_string_equals(&enc6, "Zm9vYmFy") == true);
  }

  // ─────────────────────────────────────────────────────────────────────────────
  // CLEANUP
  // ─────────────────────────────────────────────────────────────────────────────
  nya_arena_destroy(arena);

  return 0;
}
