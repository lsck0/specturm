#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  NYA_RNG rng1 = nya_rng_new();
  NYA_RNG rng2 = nya_rng_new(.seed = rng1.seed);
  NYA_RNG rng3 = nya_rng_new(.seed = "BEEF");
  (void)rng3;

  nya_assert_panic({
    NYA_RNG rng4 = nya_rng_new(.seed = "BEGgEFZ");
    (void)rng4;
  });
  nya_assert_panic({
    NYA_RNG rng5 = nya_rng_new(.seed = "AAasd786hgfasPOAISDnAAAAAAAAAAAaafsdsdsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    (void)rng5;
  });

  NYA_RNGDistribution dist_uniform = {
      .type    = NYA_RNG_DISTRIBUTION_UNIFORM,
      .uniform = {.min = 10.0, .max = 20.0},
  };
  NYA_RNGDistribution dist_normal = {
      .type   = NYA_RNG_DISTRIBUTION_NORMAL,
      .normal = {.mean = 50.0, .stddev = 10.0},
  };
  NYA_RNGDistribution dist_exponential = {
      .type        = NYA_RNG_DISTRIBUTION_EXPONENTIAL,
      .exponential = {.lambda = 0.5},
  };
  NYA_RNGDistribution dist_poisson = {
      .type    = NYA_RNG_DISTRIBUTION_POISSON,
      .poisson = {.lambda = 4.0},
  };
  NYA_RNGDistribution dist_binomial = {
      .type     = NYA_RNG_DISTRIBUTION_BINOMIAL,
      .binomial = {.n = 10, .p = 0.5},
  };
  NYA_RNGDistribution dist_geometric = {
      .type      = NYA_RNG_DISTRIBUTION_GEOMETRIC,
      .geometric = {.p = 0.3},
  };

  for (u32 counter = 0; counter < 1000; ++counter) {
    nya_assert(nya_rng_sample_f64(&rng1, dist_uniform) == nya_rng_sample_f64(&rng2, dist_uniform));
    nya_assert(nya_rng_sample_f64(&rng1, dist_normal) == nya_rng_sample_f64(&rng2, dist_normal));
    nya_assert(nya_rng_sample_f64(&rng1, dist_exponential) == nya_rng_sample_f64(&rng2, dist_exponential));
    nya_assert(nya_rng_sample_u32(&rng1, dist_poisson) == nya_rng_sample_u32(&rng2, dist_poisson));
    nya_assert(nya_rng_sample_u32(&rng1, dist_binomial) == nya_rng_sample_u32(&rng2, dist_binomial));
    nya_assert(nya_rng_sample_u32(&rng1, dist_geometric) == nya_rng_sample_u32(&rng2, dist_geometric));
  }

  return 0;
}
