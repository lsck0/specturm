//! This is a wrapper around https://espadrine.github.io/blog/posts/shishua-the-fastest-prng-in-the-world.html
#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_ints.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

typedef struct NYA_RNG             NYA_RNG;
typedef struct NYA_RNGOptions      NYA_RNGOptions;
typedef struct NYA_RNGDistribution NYA_RNGDistribution;

#define _NYA_RNG_BUFFER_SIZE     1024
#define _NYA_RNG_INIT_ROUNDS     16
#define _NYA_RNG_MAX_SEED_LENGTH 64
#define _NYA_RNG_DEFAULT_OPTIONS .seed = nullptr

struct NYA_RNGOptions {
  /// upto 64 char hex string, NULL => random seed
  const char* seed;
};

struct NYA_RNG {
  char seed[_NYA_RNG_MAX_SEED_LENGTH + 1];

  __m256i state[4];
  __m256i output[4];
  __m256i counter;

  u8  buffer[_NYA_RNG_BUFFER_SIZE];
  u64 cursor;
};

typedef enum {
  NYA_RNG_DISTRIBUTION_UNIFORM,
  NYA_RNG_DISTRIBUTION_NORMAL,
  NYA_RNG_DISTRIBUTION_EXPONENTIAL,
  NYA_RNG_DISTRIBUTION_POISSON,
  NYA_RNG_DISTRIBUTION_BINOMIAL,
  NYA_RNG_DISTRIBUTION_GEOMETRIC,
  NYA_RNG_DISTRIBUTION_COUNT,
} NYA_RNGDistributionType;

struct NYA_RNGDistribution {
  NYA_RNGDistributionType type;

  union {
    struct {
      f64 min;
      f64 max;
    } uniform;

    struct {
      f64 mean;
      f64 stddev;
    } normal;

    struct {
      f64 lambda;
    } exponential;

    struct {
      f64 lambda;
    } poisson;

    struct {
      u64 n;
      f64 p;
    } binomial;

    struct {
      f64 p;
    } geometric;
  };
};

static_assert(_NYA_RNG_BUFFER_SIZE % 128 == 0, "RNG buffer size must be a multiple of 128 bytes.");

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS AND MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_rng_new(...) nya_rng_new_with_options((NYA_RNGOptions){_NYA_RNG_DEFAULT_OPTIONS, __VA_ARGS__})
NYA_API NYA_EXTERN NYA_RNG nya_rng_new_with_options(NYA_RNGOptions options) __attr_no_discard;

NYA_API NYA_EXTERN void nya_rng_gen_bytes(NYA_RNG* rng, u8 buffer[], u64 size);
NYA_API NYA_EXTERN b8   nya_rng_gen_bool(NYA_RNG* rng, f32 true_chance) __attr_no_discard;
NYA_API NYA_EXTERN u8   nya_rng_sample_u8(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN u16  nya_rng_sample_u16(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN u32  nya_rng_sample_u32(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN u64  nya_rng_sample_u64(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN s8   nya_rng_sample_s8(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN s16  nya_rng_sample_s16(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN s32  nya_rng_sample_s32(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN s64  nya_rng_sample_s64(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN f16  nya_rng_sample_f16(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN f32  nya_rng_sample_f32(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
NYA_API NYA_EXTERN f64  nya_rng_sample_f64(NYA_RNG* rng, NYA_RNGDistribution distribution) __attr_no_discard;
