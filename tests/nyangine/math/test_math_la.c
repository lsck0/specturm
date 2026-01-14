#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  // test vector types
  f32x3 vec_a = {1.0F, 2.0F, 3.0F};

  vec_a.xy = (f32x2){4.0F, 5.0F};
  nya_assert(vec_a.x == 4.0F);
  nya_assert(vec_a.y == 5.0F);
  nya_assert(vec_a.z == 3.0F);

  vec_a += f32x3_unit_z;
  nya_assert(vec_a.x == 4.0F);
  nya_assert(vec_a.y == 5.0F);
  nya_assert(vec_a.z == 4.0F);

  vec_a = nya_lerp(vec_a, f32x3_unit_x, 0.5F);
  nya_assert(vec_a.x == 2.5F);
  nya_assert(vec_a.y == 2.5F);
  nya_assert(vec_a.z == 2.0F);

  f32x3 vec_b = f32x3_unit_y;
  nya_assert(vec_b.x == 0.0F);
  nya_assert(vec_b.y == 1.0F);
  nya_assert(vec_b.z == 0.0F);

  f32x3 vec_c = vec_a * vec_b;
  nya_assert(vec_c.x == 0.0F);
  nya_assert(vec_c.y == 2.5F);
  nya_assert(vec_c.z == 0.0F);

  // test matrix types
  f32_3x3 mat_a = f32_3x3_id;
  f32_3x3 mat_b = nya_matrix_new((f32[3][3]){
      {1.0F, 2.0F, 3.0F},
      {4.0F, 5.0F, 6.0F},
      {7.0F, 8.0F, 9.0F},
  });

  f32x3 vec_d = nya_matrix_times_vector(mat_b, f32x3_unit_z);
  nya_assert(vec_d.x == 3.0F);
  nya_assert(vec_d.y == 6.0F);
  nya_assert(vec_d.z == 9.0F);

  f32x3 vec_e = nya_matrix_times_vector(mat_a, vec_d);
  nya_assert(vec_e.x == 3.0F);
  nya_assert(vec_e.y == 6.0F);
  nya_assert(vec_e.z == 9.0F);

  f32x3 vec_f = nya_matrix_times_vector(mat_b, vec_a);
  nya_assert(vec_f.x == (1.0F * 2.5F + 2.0F * 2.5F + 3.0F * 2.0F));
  nya_assert(vec_f.y == (4.0F * 2.5F + 5.0F * 2.5F + 6.0F * 2.0F));
  nya_assert(vec_f.z == (7.0F * 2.5F + 8.0F * 2.5F + 9.0F * 2.0F));

  return 0;
}
