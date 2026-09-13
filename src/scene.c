#include "scene.h"

#include "texture.h"

#include <float.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void path_dir(const char *path, char *out, size_t size) {
  snprintf(out, size, "%s", path);
  char *slash = strrchr(out, '/');
  if (slash) {
    slash[1] = '\0';
  } else {
    out[0] = '\0';
  }
}

static GLuint material_texture(cgltf_primitive *prim, const char *baseDir) {
  if (!prim->material) {
    return 0;
  }
  cgltf_texture_view view =
      prim->material->pbr_metallic_roughness.base_color_texture;
  if (!view.texture || !view.texture->image || !view.texture->image->uri) {
    return 0;
  }
  char path[512];
  snprintf(path, sizeof(path), "%s%s", baseDir, view.texture->image->uri);
  return texture_create(path);
}

static void world_bounds(mat4 model, AABB local, AABB *out) {
  glm_vec3_copy((vec3){FLT_MAX, FLT_MAX, FLT_MAX}, out->min);
  glm_vec3_copy((vec3){-FLT_MAX, -FLT_MAX, -FLT_MAX}, out->max);

  for (int i = 0; i < 8; i++) {
    vec4 corner = {
        (i & 1) ? local.max[0] : local.min[0],
        (i & 2) ? local.max[1] : local.min[1],
        (i & 4) ? local.max[2] : local.min[2],
        1.0f,
    };
    vec4 world;
    glm_mat4_mulv(model, corner, world);
    for (int axis = 0; axis < 3; axis++) {
      if (world[axis] < out->min[axis]) {
        out->min[axis] = world[axis];
      }
      if (world[axis] > out->max[axis]) {
        out->max[axis] = world[axis];
      }
    }
  }
}

static Triangle *build_triangles(cgltf_primitive *prim, mat4 model,
                                 int *outCount) {
  cgltf_accessor *pos = NULL;
  for (cgltf_size i = 0; i < prim->attributes_count; i++) {
    if (strcmp(prim->attributes[i].name, "POSITION") == 0) {
      pos = prim->attributes[i].data;
      break;
    }
  }
  if (!pos || !prim->indices) {
    *outCount = 0;
    return NULL;
  }

  cgltf_size vertexCount = pos->count;
  vec3 *verts = malloc(vertexCount * sizeof(vec3));
  for (cgltf_size i = 0; i < vertexCount; i++) {
    float tmp[4];
    cgltf_accessor_read_float(pos, i, tmp, 3);
    vec4 local = {tmp[0], tmp[1], tmp[2], 1.0f};
    vec4 world;
    glm_mat4_mulv(model, local, world);
    verts[i][0] = world[0];
    verts[i][1] = world[1];
    verts[i][2] = world[2];
  }

  int triCount = (int)(prim->indices->count / 3);
  Triangle *tris = malloc((size_t)triCount * sizeof(Triangle));
  for (int i = 0; i < triCount; i++) {
    int i0 = (int)cgltf_accessor_read_index(prim->indices, i * 3 + 0);
    int i1 = (int)cgltf_accessor_read_index(prim->indices, i * 3 + 1);
    int i2 = (int)cgltf_accessor_read_index(prim->indices, i * 3 + 2);
    glm_vec3_copy(verts[i0], tris[i].a);
    glm_vec3_copy(verts[i1], tris[i].b);
    glm_vec3_copy(verts[i2], tris[i].c);
  }

  free(verts);
  *outCount = triCount;
  return tris;
}

int scene_load_gltf(const char *path, SceneObject **outObjects, int *outCount) {
  cgltf_options options = {0};
  cgltf_data *data = NULL;

  if (cgltf_parse_file(&options, path, &data) != cgltf_result_success) {
    fprintf(stderr, "Failed to parse %s\n", path);
    return 0;
  }
  if (cgltf_load_buffers(&options, data, path) != cgltf_result_success) {
    fprintf(stderr, "Failed to load buffers of %s\n", path);
    cgltf_free(data);
    return 0;
  }
  if (cgltf_validate(data) != cgltf_result_success) {
    fprintf(stderr, "Invalid gltf: %s\n", path);
    cgltf_free(data);
    return 0;
  }

  char baseDir[512];
  path_dir(path, baseDir, sizeof(baseDir));

  int capacity = 0;
  for (cgltf_size i = 0; i < data->nodes_count; i++) {
    if (data->nodes[i].mesh) {
      capacity += (int)data->nodes[i].mesh->primitives_count;
    }
  }

  SceneObject *objects = _aligned_malloc(capacity * sizeof(SceneObject), 32);
  int count = 0;

  for (cgltf_size i = 0; i < data->nodes_count; i++) {
    cgltf_node *node = &data->nodes[i];
    if (!node->mesh) {
      continue;
    }

    cgltf_float world[16];
    cgltf_node_transform_world(node, world);

    for (cgltf_size j = 0; j < node->mesh->primitives_count; j++) {
      cgltf_primitive *prim = &node->mesh->primitives[j];
      SceneObject *obj = &objects[count++];

      memcpy(obj->model, world, sizeof(mat4));

      obj->mesh = mesh_create_from_gltf(prim);
      obj->mesh.texture = material_texture(prim, baseDir);
      obj->tris = build_triangles(prim, obj->model, &obj->triCount);

      AABB worldBox;
      world_bounds(obj->model, obj->mesh.bounds, &worldBox);
      obj->mesh.bounds = worldBox;
    }
  }

  cgltf_free(data);

  *outObjects = objects;
  *outCount = count;
  return 1;
}

void scene_object_get_model(SceneObject *obj, mat4 dest) {
  glm_mat4_copy(obj->model, dest);
}

void scene_delete(SceneObject *objects, int count) {
  for (int i = 0; i < count; i++) {
    mesh_delete(objects[i].mesh);
    free(objects[i].tris);
  }
  _aligned_free(objects);
}
