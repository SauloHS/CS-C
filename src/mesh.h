#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

typedef struct {
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  GLuint count;
} Mesh;

Mesh cube_mesh_create(void);
Mesh floor_mesh_create(void);
void mesh_delete(Mesh mesh);

#endif
