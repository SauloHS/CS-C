#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "collision.h"

typedef struct {
  vec3 position;
  vec3 front;
  vec3 up;
  float yaw;
  float pitch;
  float lastX, lastY;
  int firstMouse;
} Camera;

Camera camera_create(void);
void camera_get_view_matrix(Camera *cam, mat4 dest);
void camera_process_mouse(Camera *cam, double xpos, double ypos);
void camera_process_keyboard(Camera *cam, GLFWwindow *window, float deltaTime,
                             AABB *sceneBounds, int sceneCount);
AABB camera_get_aabb(Camera *cam);

#endif
