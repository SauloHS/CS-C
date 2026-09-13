#ifndef COLLISION_H
#define COLLISION_H

#include <cglm/cglm.h>
#include <stdbool.h>

typedef struct {
  vec3 min;
  vec3 max;
} AABB;

bool aabb_intersect(AABB a, AABB b);

#endif
