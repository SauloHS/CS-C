#include "mesh.h"
#include <cgltf.h>
#include <stdlib.h>
#include <string.h>

static cgltf_accessor *find_attribute(cgltf_primitive *prim,
                                      const char *name) {
  for (cgltf_size i = 0; i < prim->attributes_count; i++) {
    if (strcmp(prim->attributes[i].name, name) == 0) {
      return prim->attributes[i].data;
    }
  }
  return NULL;
}

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
  mesh.isFloor = 0;

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
  mesh.isFloor = 1;
  glm_vec3_copy((vec3){-10.0f, -10.5f, -10.0f}, mesh.bounds.min);
  glm_vec3_copy((vec3){10.0f, -9.5f, 10.0f}, mesh.bounds.max);
  return mesh;
}

Mesh mesh_create_from_gltf(cgltf_primitive *prim) {
  Mesh mesh = {0};

  cgltf_accessor *pos = find_attribute(prim, "POSITION");
  cgltf_accessor *nrm = find_attribute(prim, "NORMAL");
  cgltf_accessor *uv = find_attribute(prim, "TEXCOORD_0");
  if (!pos || !prim->indices) {
    return mesh;
  }

  cgltf_size vertexCount = pos->count;
  float *vertices = malloc(vertexCount * 8 * sizeof(float));
  for (cgltf_size i = 0; i < vertexCount; i++) {
    float *v = &vertices[i * 8];
    float tmp[4];

    cgltf_accessor_read_float(pos, i, tmp, 3);
    v[0] = tmp[0];
    v[1] = tmp[1];
    v[2] = tmp[2];

    if (nrm && cgltf_accessor_read_float(nrm, i, tmp, 3)) {
      v[3] = tmp[0];
      v[4] = tmp[1];
      v[5] = tmp[2];
    } else {
      v[3] = 0.0f;
      v[4] = 1.0f;
      v[5] = 0.0f;
    }

    if (uv && cgltf_accessor_read_float(uv, i, tmp, 2)) {
      v[6] = tmp[0];
      v[7] = tmp[1];
    } else {
      v[6] = 0.0f;
      v[7] = 0.0f;
    }
  }

  cgltf_size indexCount = prim->indices->count;
  GLuint *indices = malloc(indexCount * sizeof(GLuint));
  for (cgltf_size i = 0; i < indexCount; i++) {
    indices[i] = (GLuint)cgltf_accessor_read_index(prim->indices, i);
  }

  mesh.count = (GLuint)indexCount;
  for (int i = 0; i < 3; i++) {
    mesh.bounds.min[i] = pos->min[i];
    mesh.bounds.max[i] = pos->max[i];
  }

  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);

  glBindVertexArray(mesh.VAO);

  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, vertexCount * 8 * sizeof(float), vertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices,
               GL_STATIC_DRAW);

  GLsizei stride = 8 * sizeof(float);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  free(vertices);
  free(indices);
  return mesh;
}

void mesh_delete(Mesh mesh) {
  glDeleteVertexArrays(1, &mesh.VAO);
  glDeleteBuffers(1, &mesh.VBO);
  glDeleteBuffers(1, &mesh.EBO);
}
