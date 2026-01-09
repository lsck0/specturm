#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

/*
 * ─────────────────────────────────────────────────────────
 * MATRIX CONSTRUCTORS
 * ─────────────────────────────────────────────────────────
 */

f16_2x2 nya_matrix_new(f16x2 row1, f16x2 row2) __attr_overloaded {
  f16_2x2 result;

  result[0][0] = row1[0];
  result[0][1] = row1[1];
  result[1][0] = row2[0];
  result[1][1] = row2[1];

  return result;
}

f16_3x3 nya_matrix_new(f16x3 row1, f16x3 row2, f16x3 row3) __attr_overloaded {
  f16_3x3 result;

  result[0][0] = row1[0];
  result[0][1] = row1[1];
  result[0][2] = row1[2];
  result[1][0] = row2[0];
  result[1][1] = row2[1];
  result[1][2] = row2[2];
  result[2][0] = row3[0];
  result[2][1] = row3[1];
  result[2][2] = row3[2];

  return result;
}

f16_4x4 nya_matrix_new(f16x4 row1, f16x4 row2, f16x4 row3, f16x4 row4) __attr_overloaded {
  f16_4x4 result;

  result[0][0] = row1[0];
  result[0][1] = row1[1];
  result[0][2] = row1[2];
  result[0][3] = row1[3];
  result[1][0] = row2[0];
  result[1][1] = row2[1];
  result[1][2] = row2[2];
  result[1][3] = row2[3];
  result[2][0] = row3[0];
  result[2][1] = row3[1];
  result[2][2] = row3[2];
  result[2][3] = row3[3];
  result[3][0] = row4[0];
  result[3][1] = row4[1];
  result[3][2] = row4[2];
  result[3][3] = row4[3];

  return result;
}

f32_2x2 nya_matrix_new(f32x2 row1, f32x2 row2) __attr_overloaded {
  f32_2x2 result;

  result[0][0] = row1[0];
  result[0][1] = row1[1];
  result[1][0] = row2[0];
  result[1][1] = row2[1];

  return result;
}

f32_3x3 nya_matrix_new(f32x3 row1, f32x3 row2, f32x3 row3) __attr_overloaded {
  f32_3x3 result;

  result[0][0] = row1[0];
  result[0][1] = row1[1];
  result[0][2] = row1[2];
  result[1][0] = row2[0];
  result[1][1] = row2[1];
  result[1][2] = row2[2];
  result[2][0] = row3[0];
  result[2][1] = row3[1];
  result[2][2] = row3[2];

  return result;
}

f32_4x4 nya_matrix_new(f32x4 row1, f32x4 row2, f32x4 row3, f32x4 row4) __attr_overloaded {
  f32_4x4 result;

  result[0][0] = row1[0];
  result[0][1] = row1[1];
  result[0][2] = row1[2];
  result[0][3] = row1[3];
  result[1][0] = row2[0];
  result[1][1] = row2[1];
  result[1][2] = row2[2];
  result[1][3] = row2[3];
  result[2][0] = row3[0];
  result[2][1] = row3[1];
  result[2][2] = row3[2];
  result[2][3] = row3[3];
  result[3][0] = row4[0];
  result[3][1] = row4[1];
  result[3][2] = row4[2];
  result[3][3] = row4[3];

  return result;
}

f64_2x2 nya_matrix_new(f64x2 row1, f64x2 row2) __attr_overloaded {
  f64_2x2 result;

  result[0][0] = row1[0];
  result[0][1] = row1[1];
  result[1][0] = row2[0];
  result[1][1] = row2[1];

  return result;
}

f64_3x3 nya_matrix_new(f64x3 row1, f64x3 row2, f64x3 row3) __attr_overloaded {
  f64_3x3 result;

  result[0][0] = row1[0];
  result[0][1] = row1[1];
  result[0][2] = row1[2];
  result[1][0] = row2[0];
  result[1][1] = row2[1];
  result[1][2] = row2[2];
  result[2][0] = row3[0];
  result[2][1] = row3[1];
  result[2][2] = row3[2];

  return result;
}

f64_4x4 nya_matrix_new(f64x4 row1, f64x4 row2, f64x4 row3, f64x4 row4) __attr_overloaded {
  f64_4x4 result;

  result[0][0] = row1[0];
  result[0][1] = row1[1];
  result[0][2] = row1[2];
  result[0][3] = row1[3];
  result[1][0] = row2[0];
  result[1][1] = row2[1];
  result[1][2] = row2[2];
  result[1][3] = row2[3];
  result[2][0] = row3[0];
  result[2][1] = row3[1];
  result[2][2] = row3[2];
  result[2][3] = row3[3];
  result[3][0] = row4[0];
  result[3][1] = row4[1];
  result[3][2] = row4[2];
  result[3][3] = row4[3];

  return result;
}

/*
 * ─────────────────────────────────────────────────────────
 * MATRIX VECTOR MULTIPLICATION
 * ─────────────────────────────────────────────────────────
 */

f16x2 nya_matrix_times_vector(f16_2x2 mat, f16x2 vec) __attr_overloaded {
  return (f16x2){
      mat[0][0] * vec[0] + mat[0][1] * vec[1],
      mat[1][0] * vec[0] + mat[1][1] * vec[1],
  };
}

f16x3 nya_matrix_times_vector(f16_3x3 mat, f16x3 vec) __attr_overloaded {
  return (f16x3){
      mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2],
      mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2],
      mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2],
  };
}

f16x4 nya_matrix_times_vector(f16_4x4 mat, f16x4 vec) __attr_overloaded {
  return (f16x4){
      mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3] * vec[3],
      mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3] * vec[3],
      mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] * vec[3],
      mat[3][0] * vec[0] + mat[3][1] * vec[1] + mat[3][2] * vec[2] + mat[3][3] * vec[3],
  };
}

f32x2 nya_matrix_times_vector(f32_2x2 mat, f32x2 vec) __attr_overloaded {
  return (f32x2){
      mat[0][0] * vec[0] + mat[0][1] * vec[1],
      mat[1][0] * vec[0] + mat[1][1] * vec[1],
  };
}

f32x3 nya_matrix_times_vector(f32_3x3 mat, f32x3 vec) __attr_overloaded {
  return (f32x3){
      mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2],
      mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2],
      mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2],
  };
}

f32x4 nya_matrix_times_vector(f32_4x4 mat, f32x4 vec) __attr_overloaded {
  return (f32x4){
      mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3] * vec[3],
      mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3] * vec[3],
      mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] * vec[3],
      mat[3][0] * vec[0] + mat[3][1] * vec[1] + mat[3][2] * vec[2] + mat[3][3] * vec[3],
  };
}

f64x2 nya_matrix_times_vector(f64_2x2 mat, f64x2 vec) __attr_overloaded {
  return (f64x2){
      mat[0][0] * vec[0] + mat[0][1] * vec[1],
      mat[1][0] * vec[0] + mat[1][1] * vec[1],
  };
}

f64x3 nya_matrix_times_vector(f64_3x3 mat, f64x3 vec) __attr_overloaded {
  return (f64x3){
      mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2],
      mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2],
      mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2],
  };
}

f64x4 nya_matrix_times_vector(f64_4x4 mat, f64x4 vec) __attr_overloaded {
  return (f64x4){
      mat[0][0] * vec[0] + mat[0][1] * vec[1] + mat[0][2] * vec[2] + mat[0][3] * vec[3],
      mat[1][0] * vec[0] + mat[1][1] * vec[1] + mat[1][2] * vec[2] + mat[1][3] * vec[3],
      mat[2][0] * vec[0] + mat[2][1] * vec[1] + mat[2][2] * vec[2] + mat[2][3] * vec[3],
      mat[3][0] * vec[0] + mat[3][1] * vec[1] + mat[3][2] * vec[2] + mat[3][3] * vec[3],
  };
}
