#ifndef COLLISION_H
#define COLLISION_H

#include <cglm/cglm.h>
#include <stdbool.h>

typedef struct {
  vec3 min;
  vec3 max;
} AABB;

typedef struct {
  vec3 a, b, c;
} Triangle;

typedef struct {
  const Triangle *tris;
  int triCount;
  AABB bounds;
} CollisionMesh;

bool aabb_intersect(AABB a, AABB b);
bool triangle_aabb_intersect(Triangle t, AABB box);

#endif
