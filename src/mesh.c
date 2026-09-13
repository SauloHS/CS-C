#include "mesh.h"

Mesh cube_mesh_create(void) {
  // clang-format off
  GLfloat vertices[] = {
      // position           // UV
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // 0
      0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, // 1
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, // 2
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, // 3
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, // 4
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, // 5
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 6
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, // 7
  };
  // clang-format on

  GLuint index[] = {
      // behind
      0,
      1,
      2,
      2,
      3,
      0,
      // front
      4,
      5,
      6,
      6,
      7,
      4,
      // left
      4,
      0,
      3,
      3,
      7,
      4,
      // right
      1,
      5,
      6,
      6,
      2,
      1,
      // below
      4,
      5,
      1,
      1,
      0,
      4,
      // up
      3,
      2,
      6,
      6,
      7,
      3,
  };

  Mesh mesh;

  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);

  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  mesh.count = sizeof(index) / sizeof(GLuint);

  // temporary; real bounds will come from Blender
  glm_vec3_copy((vec3){-0.5f, -0.5f, -3.5f}, mesh.bounds.min);
  glm_vec3_copy((vec3){0.5f, 0.5f, -2.5f}, mesh.bounds.max);
  return mesh;
}

Mesh floor_mesh_create(void) {
  GLfloat vertices[] = {
      -10.0f, -0.5f, -10.0f, 10.0f,  -0.5f, -10.0f,
      10.0f,  -0.5f, 10.0f,  -10.0f, -0.5f, 10.0f,
  };

  GLuint indices[] = {
      0, 1, 2, 2, 3, 0,
  };

  Mesh mesh;
  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);

  glBindVertexArray(mesh.VAO);

  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  mesh.count = 6;
  glm_vec3_copy((vec3){-10.0f, -10.5f, -10.0f}, mesh.bounds.min);
  glm_vec3_copy((vec3){10.0f, -10.4f, 10.0f}, mesh.bounds.max);
  return mesh;
}

void mesh_delete(Mesh mesh) {
  glDeleteVertexArrays(1, &mesh.VAO);
  glDeleteBuffers(1, &mesh.VBO);
}
