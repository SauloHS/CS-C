#include "scene.h"

void scene_object_get_model(SceneObject *obj, mat4 dest) {
  glm_mat4_identity(dest);
  glm_translate(dest, obj->position);
  glm_rotate(dest, glm_rad(obj->rotationY), (vec3){0.0f, 1.0f, 0.0f});
}
