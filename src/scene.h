#ifndef SCENE_H
#define SCENE_H

#include "mesh.h"
#include <cglm/cglm.h>

typedef struct {
  Mesh mesh;
  mat4 model;
  Triangle *tris;
  int triCount;
} SceneObject;

int scene_load_gltf(const char *path, SceneObject **outObjects, int *outCount);
void scene_object_get_model(SceneObject *obj, mat4 dest);
void scene_delete(SceneObject *objects, int count);

#endif
