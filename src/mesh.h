#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include "collision.h"
#include <cgltf.h>

typedef struct {
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  GLuint count;
  GLuint texture;
  AABB bounds;
  int isFloor;
} Mesh;

Mesh cube_mesh_create(void);
Mesh floor_mesh_create(void);
Mesh mesh_create_from_gltf(cgltf_primitive *prim);
void mesh_delete(Mesh mesh);

#endif
