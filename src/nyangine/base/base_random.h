//! This is a wrapper around https://espadrine.github.io/blog/posts/shishua-the-fastest-prng-in-the-world.html.
#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_ints.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_RNG        NYA_RNG;
typedef struct NYA_RNGOptions NYA_RNGOptions;

#define _NYA_RNG_BUFFER_SIZE     1024
#define _NYA_RNG_INIT_ROUNDS     16
#define _NYA_RNG_DEFAULT_OPTIONS .seed = {0, 0, 0, 0}

// TODO: take the seed in a different format, like a string A-Za-z-0-9 and convert it to u64[4] internally
struct NYA_RNGOptions {
  u64 seed[4]; // seed == 0 => take random seed
};

struct NYA_RNG {
  u64 seed[4];

  __m256i state[4];
  __m256i output[4];
  __m256i counter;

  u8  buffer[_NYA_RNG_BUFFER_SIZE];
  u64 cursor;
};

static_assert(_NYA_RNG_BUFFER_SIZE % 128 == 0, "RNG buffer size must be a multiple of 128 bytes.");

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS AND MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_rng_new(...) _nya_rng_new_with_options((NYA_RNGOptions){_NYA_RNG_DEFAULT_OPTIONS, __VA_ARGS__})
NYA_API NYA_EXTERN NYA_RNG nya_rng_new_with_options(NYA_RNGOptions options) __attr_no_discard;

// TODO: that api is terrible lmao: do sampling from distributions and generate random bytes etc, similar to rust rng
NYA_API NYA_EXTERN b8  nya_rng_gen_b8(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN b16 nya_rng_gen_b16(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN b32 nya_rng_gen_b32(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN b64 nya_rng_gen_b64(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN u8  nya_rng_gen_u8(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN u16 nya_rng_gen_u16(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN u32 nya_rng_gen_u32(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN u64 nya_rng_gen_u64(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN s8  nya_rng_gen_s8(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN s16 nya_rng_gen_s16(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN s32 nya_rng_gen_s32(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN s64 nya_rng_gen_s64(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN f16 nya_rng_gen_f16(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN f32 nya_rng_gen_f32(NYA_RNG* rng) __attr_no_discard;
NYA_API NYA_EXTERN f64 nya_rng_gen_f64(NYA_RNG* rng) __attr_no_discard;
