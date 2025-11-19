#pragma once

#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_ints.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * VECTOR TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if defined(__has_feature) && __has_attribute(ext_vector_type)
typedef f16 f16x2 __attr_vector(2);
typedef f16 f16x3 __attr_vector(3);
typedef f16 f16x4 __attr_vector(4);
typedef f32 f32x2 __attr_vector(2);
typedef f32 f32x3 __attr_vector(3);
typedef f32 f32x4 __attr_vector(4);
typedef f64 f64x2 __attr_vector(2);
typedef f64 f64x3 __attr_vector(3);
typedef f64 f64x4 __attr_vector(4);

#define FMTf16x2          "(" FMTf16 ", " FMTf16 ")"
#define FMTf16x3          "(" FMTf16 ", " FMTf16 ", " FMTf16 ")"
#define FMTf16x4          "(" FMTf16 ", " FMTf16 ", " FMTf16 ", " FMTf16 ")"
#define FMTf16x2_ARG(val) (val).x, (val).y
#define FMTf16x3_ARG(val) (val).x, (val).y, (val).z
#define FMTf16x4_ARG(val) (val).x, (val).y, (val).z, (val).w
#define FMTf32x2          "(" FMTf32 ", " FMTf32 ")"
#define FMTf32x3          "(" FMTf32 ", " FMTf32 ", " FMTf32 ")"
#define FMTf32x4          "(" FMTf32 ", " FMTf32 ", " FMTf32 ", " FMTf32 ")"
#define FMTf32x2_ARG(val) (val).x, (val).y
#define FMTf32x3_ARG(val) (val).x, (val).y, (val).z
#define FMTf32x4_ARG(val) (val).x, (val).y, (val).z, (val).w
#define FMTf64x2          "(" FMTf64 ", " FMTf64 ")"
#define FMTf64x3          "(" FMTf64 ", " FMTf64 ", " FMTf64 ")"
#define FMTf64x4          "(" FMTf64 ", " FMTf64 ", " FMTf64 ", " FMTf64 ")"
#define FMTf64x2_ARG(val) (val).x, (val).y
#define FMTf64x3_ARG(val) (val).x, (val).y, (val).z
#define FMTf64x4_ARG(val) (val).x, (val).y, (val).z, (val).w

#define f16x2_zero ((f16x2){0, 0})
#define f16x3_zero ((f16x3){0, 0, 0})
#define f16x4_zero ((f16x4){0, 0, 0, 0})
#define f32x2_zero ((f32x2){0, 0})
#define f32x3_zero ((f32x3){0, 0, 0})
#define f32x4_zero ((f32x4){0, 0, 0, 0})
#define f64x2_zero ((f64x2){0, 0})
#define f64x3_zero ((f64x3){0, 0, 0})
#define f64x4_zero ((f64x4){0, 0, 0, 0})

#define f16x2_unit_x ((f16x2){1, 0})
#define f16x2_unit_y ((f16x2){0, 1})
#define f32x2_unit_x ((f32x2){1, 0})
#define f32x2_unit_y ((f32x2){0, 1})
#define f64x2_unit_x ((f64x2){1, 0})
#define f64x2_unit_y ((f64x2){0, 1})

#define f16x3_unit_x ((f16x3){1, 0, 0})
#define f16x3_unit_y ((f16x3){0, 1, 0})
#define f16x3_unit_z ((f16x3){0, 0, 1})
#define f32x3_unit_x ((f32x3){1, 0, 0})
#define f32x3_unit_y ((f32x3){0, 1, 0})
#define f32x3_unit_z ((f32x3){0, 0, 1})
#define f64x3_unit_x ((f64x3){1, 0, 0})
#define f64x3_unit_y ((f64x3){0, 1, 0})
#define f64x3_unit_z ((f64x3){0, 0, 1})

#define f16x4_unit_x ((f16x4){1, 0, 0, 0})
#define f16x4_unit_y ((f16x4){0, 1, 0, 0})
#define f16x4_unit_z ((f16x4){0, 0, 1, 0})
#define f16x4_unit_w ((f16x4){0, 0, 0, 1})
#define f32x4_unit_x ((f32x4){1, 0, 0, 0})
#define f32x4_unit_y ((f32x4){0, 1, 0, 0})
#define f32x4_unit_z ((f32x4){0, 0, 1, 0})
#define f32x4_unit_w ((f32x4){0, 0, 0, 1})
#define f64x4_unit_x ((f64x4){1, 0, 0, 0})
#define f64x4_unit_y ((f64x4){0, 1, 0, 0})
#define f64x4_unit_z ((f64x4){0, 0, 1, 0})
#define f64x4_unit_w ((f64x4){0, 0, 0, 1})
#endif // defined(__has_feature) && __has_attribute(ext_vector_type)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * MATRIX TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if defined(__has_feature) && __has_attribute(ext_vector_type) && __has_attribute(matrix_type)
typedef f16 f16_2x2 __attr_matrix(2, 2);
typedef f16 f16_3x3 __attr_matrix(3, 3);
typedef f16 f16_4x4 __attr_matrix(4, 4);
typedef f32 f32_2x2 __attr_matrix(2, 2);
typedef f32 f32_3x3 __attr_matrix(3, 3);
typedef f32 f32_4x4 __attr_matrix(4, 4);
typedef f64 f64_2x2 __attr_matrix(2, 2);
typedef f64 f64_3x3 __attr_matrix(3, 3);
typedef f64 f64_4x4 __attr_matrix(4, 4);

#define f16_2x2_zero (nya_new_matrix(f16x2_zero, f16x2_zero))
#define f16_3x3_zero (nya_new_matrix(f16x3_zero, f16x3_zero, f16x3_zero))
#define f16_4x4_zero (nya_new_matrix(f16x4_zero, f16x4_zero, f16x4_zero, f16x4_zero))
#define f32_2x2_zero (nya_new_matrix(f32x2_zero, f32x2_zero))
#define f32_3x3_zero (nya_new_matrix(f32x3_zero, f32x3_zero, f32x3_zero))
#define f32_4x4_zero (nya_new_matrix(f32x4_zero, f32x4_zero, f32x4_zero, f32x4_zero))
#define f64_2x2_zero (nya_new_matrix(f64x2_zero, f64x2_zero))
#define f64_3x3_zero (nya_new_matrix(f64x3_zero, f64x3_zero, f64x3_zero))
#define f64_4x4_zero (nya_new_matrix(f64x4_zero, f64x4_zero, f64x4_zero, f64x4_zero))

#define f16_2x2_id (nya_new_matrix(f16x2_unit_x, f16x2_unit_y))
#define f16_3x3_id (nya_new_matrix(f16x3_unit_x, f16x3_unit_y, f16x3_unit_z))
#define f16_4x4_id (nya_new_matrix(f16x4_unit_x, f16x4_unit_y, f16x4_unit_z, f16x4_unit_w))
#define f32_2x2_id (nya_new_matrix(f32x2_unit_x, f32x2_unit_y))
#define f32_3x3_id (nya_new_matrix(f32x3_unit_x, f32x3_unit_y, f32x3_unit_z))
#define f32_4x4_id (nya_new_matrix(f32x4_unit_x, f32x4_unit_y, f32x4_unit_z, f32x4_unit_w))
#define f64_2x2_id (nya_new_matrix(f64x2_unit_x, f64x2_unit_y))
#define f64_3x3_id (nya_new_matrix(f64x3_unit_x, f64x3_unit_y, f64x3_unit_z))
#define f64_4x4_id (nya_new_matrix(f64x4_unit_x, f64x4_unit_y, f64x4_unit_z, f64x4_unit_w))

NYA_API NYA_EXTERN f16_2x2 nya_new_matrix(f16x2 row1, f16x2 row2) __attr_overloaded;
NYA_API NYA_EXTERN f16_3x3 nya_new_matrix(f16x3 row1, f16x3 row2, f16x3 row3) __attr_overloaded;
NYA_API NYA_EXTERN f16_4x4 nya_new_matrix(f16x4 row1, f16x4 row2, f16x4 row3, f16x4 row4) __attr_overloaded;
NYA_API NYA_EXTERN f32_2x2 nya_new_matrix(f32x2 row1, f32x2 row2) __attr_overloaded;
NYA_API NYA_EXTERN f32_3x3 nya_new_matrix(f32x3 row1, f32x3 row2, f32x3 row3) __attr_overloaded;
NYA_API NYA_EXTERN f32_4x4 nya_new_matrix(f32x4 row1, f32x4 row2, f32x4 row3, f32x4 row4) __attr_overloaded;
NYA_API NYA_EXTERN f64_2x2 nya_new_matrix(f64x2 row1, f64x2 row2) __attr_overloaded;
NYA_API NYA_EXTERN f64_3x3 nya_new_matrix(f64x3 row1, f64x3 row2, f64x3 row3) __attr_overloaded;
NYA_API NYA_EXTERN f64_4x4 nya_new_matrix(f64x4 row1, f64x4 row2, f64x4 row3, f64x4 row4) __attr_overloaded;

NYA_API NYA_EXTERN f16x2 nya_matrix_times_vector(f16_2x2 mat, f16x2 vec) __attr_overloaded;
NYA_API NYA_EXTERN f16x3 nya_matrix_times_vector(f16_3x3 mat, f16x3 vec) __attr_overloaded;
NYA_API NYA_EXTERN f16x4 nya_matrix_times_vector(f16_4x4 mat, f16x4 vec) __attr_overloaded;
NYA_API NYA_EXTERN f32x2 nya_matrix_times_vector(f32_2x2 mat, f32x2 vec) __attr_overloaded;
NYA_API NYA_EXTERN f32x3 nya_matrix_times_vector(f32_3x3 mat, f32x3 vec) __attr_overloaded;
NYA_API NYA_EXTERN f32x4 nya_matrix_times_vector(f32_4x4 mat, f32x4 vec) __attr_overloaded;
NYA_API NYA_EXTERN f64x2 nya_matrix_times_vector(f64_2x2 mat, f64x2 vec) __attr_overloaded;
NYA_API NYA_EXTERN f64x3 nya_matrix_times_vector(f64_3x3 mat, f64x3 vec) __attr_overloaded;
NYA_API NYA_EXTERN f64x4 nya_matrix_times_vector(f64_4x4 mat, f64x4 vec) __attr_overloaded;
#endif // defined(__has_feature) && __has_attribute(ext_vector_type) && __has_attribute(matrix_type)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * UTILITIES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_min(a, b)                                                                                                  \
  ({                                                                                                                   \
    nya_assert_type_match(a, b);                                                                                       \
    (a) < (b) ? (a) : (b);                                                                                             \
  })
#define nya_max(a, b)                                                                                                  \
  ({                                                                                                                   \
    nya_assert_type_match(a, b);                                                                                       \
    (a) > (b) ? (a) : (b);                                                                                             \
  })
#define nya_clamp(val, min, max)                                                                                       \
  ({                                                                                                                   \
    nya_assert_type_match(val, min);                                                                                   \
    nya_assert_type_match(val, max);                                                                                   \
    nya_min(nya_max((val), (min)), (max));                                                                             \
  })
#define nya_lerp(a, b, t)                                                                                              \
  ({                                                                                                                   \
    nya_assert_type_match(a, b);                                                                                       \
    nya_assert_type_match(a, t);                                                                                       \
    nya_assert_type_match(b, t);                                                                                       \
    (a) + ((b) - (a)) * (t);                                                                                           \
  })
