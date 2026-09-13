#include "collision.h"
#include <cglm/cglm.h>
#include <math.h>

bool aabb_intersect(AABB a, AABB b) {
  return (a.min[0] <= b.max[0] && a.max[0] >= b.min[0]) &&
         (a.min[1] <= b.max[1] && a.max[1] >= b.min[1]) &&
         (a.min[2] <= b.max[2] && a.max[2] >= b.min[2]);
}

static void axis_range(vec3 v0, vec3 v1, vec3 v2, vec3 axis, float *min,
                       float *max, float *radius, vec3 e) {
  float p0 = glm_vec3_dot(axis, v0);
  float p1 = glm_vec3_dot(axis, v1);
  float p2 = glm_vec3_dot(axis, v2);
  *min = p0 < p1 ? (p0 < p2 ? p0 : p2) : (p1 < p2 ? p1 : p2);
  *max = p0 > p1 ? (p0 > p2 ? p0 : p2) : (p1 > p2 ? p1 : p2);
  *radius = e[0] * fabsf(axis[0]) + e[1] * fabsf(axis[1]) +
            e[2] * fabsf(axis[2]);
}

bool triangle_aabb_intersect(Triangle t, AABB box) {
  vec3 c, e;
  for (int i = 0; i < 3; i++) {
    c[i] = (box.min[i] + box.max[i]) * 0.5f;
    e[i] = (box.max[i] - box.min[i]) * 0.5f;
  }

  vec3 v0, v1, v2;
  for (int i = 0; i < 3; i++) {
    v0[i] = t.a[i] - c[i];
    v1[i] = t.b[i] - c[i];
    v2[i] = t.c[i] - c[i];
  }

  vec3 f[3];
  glm_vec3_sub(v1, v0, f[0]);
  glm_vec3_sub(v2, v1, f[1]);
  glm_vec3_sub(v0, v2, f[2]);

  for (int axis = 0; axis < 3; axis++) {
    vec3 boxAxis = {0.0f, 0.0f, 0.0f};
    boxAxis[axis] = 1.0f;
    float min, max, radius;
    axis_range(v0, v1, v2, boxAxis, &min, &max, &radius, e);
    if (min > radius || max < -radius) {
      return false;
    }
  }

  vec3 normal;
  glm_vec3_cross(f[0], f[1], normal);
  float min, max, radius;
  axis_range(v0, v1, v2, normal, &min, &max, &radius, e);
  if (min > radius || max < -radius) {
    return false;
  }

  for (int edge = 0; edge < 3; edge++) {
    for (int axis = 0; axis < 3; axis++) {
      vec3 boxAxis = {0.0f, 0.0f, 0.0f};
      boxAxis[axis] = 1.0f;
      vec3 crossAxis;
      glm_vec3_cross(boxAxis, f[edge], crossAxis);
      axis_range(v0, v1, v2, crossAxis, &min, &max, &radius, e);
      if (min > radius || max < -radius) {
        return false;
      }
    }
  }

  return true;
}
