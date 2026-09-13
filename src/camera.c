#include "camera.h"
#include "cglm/vec3.h"

/* Defines `sensitivity` */
#include "variables.h"
/*======================*/
#include <math.h>

Camera camera_create(void) {
  Camera cam;

  cam.position[0] = 0.0f;
  cam.position[1] = 0.0f;
  cam.position[2] = 3.0f;

  cam.front[0] = 0.0f;
  cam.front[1] = 0.0f;
  cam.front[2] = -1.0f;

  cam.up[0] = 0.0f;
  cam.up[1] = 1.0f;
  cam.up[2] = 0.0f;

  cam.yaw = -90.0f;
  cam.pitch = 0.0f;
  cam.lastX = 400.0f;
  cam.lastY = 300.0f;
  cam.firstMouse = 1;

  return cam;
}

// Calculate camera view matrix and put it on dest
void camera_get_view_matrix(Camera *cam, mat4 dest) {
  vec3 center;
  glm_vec3_add(cam->position, cam->front, center);
  glm_lookat(cam->position, center, cam->up, dest);
}

static void camera_update_vectors(Camera *cam) {
  vec3 front;
  front[0] = cosf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
  front[1] = sinf(glm_rad(cam->pitch));
  front[2] = sinf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));

  glm_vec3_normalize(front);
  glm_vec3_copy(front, cam->front);
}

void camera_process_mouse(Camera *cam, double xpos, double ypos) {
  if (cam->firstMouse) {
    cam->lastX = (float)xpos;
    cam->lastY = (float)ypos;
    cam->firstMouse = 0;
  }

  float xoffset = (float)xpos - cam->lastX;
  float yoffset = cam->lastY - (float)ypos;
  cam->lastX = (float)xpos;
  cam->lastY = (float)ypos;

  xoffset *= sensitivity;
  yoffset *= sensitivity;

  cam->yaw += xoffset;
  cam->pitch += yoffset;

  if (cam->pitch > 89.0f)
    cam->pitch = 89.0f;
  if (cam->pitch < -89.0f)
    cam->pitch = -89.0f;

  camera_update_vectors(cam);
}

void camera_process_keyboard(Camera *cam, GLFWwindow *window, float deltaTime) {
  float velocity = 2.5f * deltaTime;

  vec3 right;
  glm_vec3_cross(cam->front, cam->up, right);
  glm_vec3_normalize(right);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    vec3 move;
    glm_vec3_scale(cam->front, velocity, move);
    glm_vec3_add(cam->position, move, cam->position);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    vec3 move;
    glm_vec3_scale(cam->front, velocity, move);
    glm_vec3_sub(cam->position, move, cam->position);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    vec3 move;
    glm_vec3_scale(right, velocity, move);
    glm_vec3_sub(cam->position, move, cam->position);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    vec3 move;
    glm_vec3_scale(right, velocity, move);
    glm_vec3_add(cam->position, move, cam->position);
  }
}
