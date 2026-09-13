#ifndef SCENE_H
#define SCENE_H

#include "mesh.h"
#include <cglm/cglm.h>

typedef struct {
  Mesh mesh;
  vec3 position;
  float rotationY;
} SceneObject;

void scene_object_get_model(SceneObject *obj, mat4 dest);
#endif
