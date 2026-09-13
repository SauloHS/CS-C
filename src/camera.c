#include "camera.h"
#include "GLFW/glfw3.h"
#include "cglm/vec3.h"

/* Defines `sensitivity` */
#include "collision.h"
#include "variables.h"
/*======================*/
#include <math.h>

#define GRAVITY 20.0f
#define JUMP_FORCE 6.0f
#define CROUCH_SPEED 8.0f
#define CROUCH_AMOUNT 0.5f
#define STEP_HEIGHT 0.55f
#define UNITS_PER_METER 1.0f
#define WALK_SPEED (6.35f * UNITS_PER_METER)
#define PLAYER_HEIGHT_STANDING 1.83f
#define PLAYER_HEIGHT_CROUCHED 1.37f
#define EYE_HEIGHT_STANDING 1.62f
#define EYE_HEIGHT_CROUCHED                                                    \
  (EYE_HEIGHT_STANDING - (PLAYER_HEIGHT_STANDING - PLAYER_HEIGHT_CROUCHED))

Camera camera_create(void) {
  Camera cam;

  cam.position[0] = 0.0f;
  cam.position[1] = 10.0f;
  cam.position[2] = 10.0f;

  cam.front[0] = -0.19f;
  cam.front[1] = -0.16f;
  cam.front[2] = -0.97f;

  cam.up[0] = 0.0f;
  cam.up[1] = 1.0f;
  cam.up[2] = 0.0f;

  cam.yaw = -101.0f;
  cam.pitch = -9.0f;
  cam.lastX = 400.0f;
  cam.lastY = 300.0f;
  cam.firstMouse = 1;

  cam.velocityY = 0.0f;
  cam.onGround = 0;
  cam.isCrouching = 0;
  cam.crouchOffset = 0.0f;

  return cam;
}

AABB camera_get_aabb(Camera *cam) {
  AABB box;
  float halfWidth = 0.3f;
  float height =
      PLAYER_HEIGHT_STANDING -
      (cam->crouchOffset * (PLAYER_HEIGHT_STANDING - PLAYER_HEIGHT_CROUCHED) /
       CROUCH_AMOUNT);

  box.min[0] = cam->position[0] - halfWidth;
  box.min[1] = cam->position[1];
  box.min[2] = cam->position[2] - halfWidth;

  box.max[0] = cam->position[0] + halfWidth;
  box.max[1] = cam->position[1] + height;
  box.max[2] = cam->position[2] + halfWidth;

  return box;
}

// Calculate camera view matrix and put it on dest
void camera_get_view_matrix(Camera *cam, mat4 dest) {
  float eyeOffset =
      EYE_HEIGHT_STANDING -
      (cam->crouchOffset * (EYE_HEIGHT_STANDING - EYE_HEIGHT_CROUCHED) /
       CROUCH_AMOUNT);

  vec3 eyePosition;
  glm_vec3_copy(cam->position, eyePosition);
  eyePosition[1] += eyeOffset;

  vec3 center;
  glm_vec3_add(eyePosition, cam->front, center);
  glm_lookat(eyePosition, center, cam->up, dest);
}

static void camera_update_vectors(Camera *cam) {
  vec3 front;
  front[0] = cosf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
  front[1] = sinf(glm_rad(cam->pitch));
  front[2] = sinf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));

  glm_vec3_normalize(front);
  glm_vec3_copy(front, cam->front);
}

static bool player_blocked_at(Camera *cam, vec3 pos, CollisionMesh *scene,
                              int sceneCount) {
  Camera testCam = *cam;
  glm_vec3_copy(pos, testCam.position);
  AABB box = camera_get_aabb(&testCam);

  for (int i = 0; i < sceneCount; i++) {
    if (!aabb_intersect(box, scene[i].bounds)) {
      continue;
    }
    for (int t = 0; t < scene[i].triCount; t++) {
      if (triangle_aabb_intersect(scene[i].tris[t], box)) {
        return true;
      }
    }
  }
  return false;
}

static void camera_move_axis(Camera *cam, int axis, float delta,
                             CollisionMesh *scene, int sceneCount) {
  vec3 target;
  glm_vec3_copy(cam->position, target);
  target[axis] += delta;

  if (!player_blocked_at(cam, target, scene, sceneCount)) {
    cam->position[axis] = target[axis];
    return;
  }

  if (!cam->onGround) {
    return;
  }

  target[1] += STEP_HEIGHT;
  if (player_blocked_at(cam, target, scene, sceneCount)) {
    return;
  }

  cam->position[axis] = target[axis];

  float baseY = cam->position[1];
  float y = baseY + STEP_HEIGHT;
  while (y > baseY) {
    float nextY = y - 0.05f;
    if (nextY < baseY) {
      nextY = baseY;
    }
    vec3 test;
    glm_vec3_copy(cam->position, test);
    test[1] = nextY;
    if (player_blocked_at(cam, test, scene, sceneCount)) {
      break;
    }
    y = nextY;
  }
  cam->position[1] = y;
}

static int camera_check_ground(Camera *cam, CollisionMesh *scene,
                               int sceneCount) {
  vec3 probePos;
  glm_vec3_copy(cam->position, probePos);
  probePos[1] -= 0.05f;
  return player_blocked_at(cam, probePos, scene, sceneCount);
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

void camera_process_keyboard(Camera *cam, GLFWwindow *window, float deltaTime,
                             CollisionMesh *scene, int sceneCount) {

  cam->isCrouching = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
  float velocity = WALK_SPEED * deltaTime;
  if (cam->isCrouching) {
    velocity *= 0.5f;
  }

  float crouchTarget = cam->isCrouching ? CROUCH_AMOUNT : 0.0f;

  if (cam->crouchOffset < crouchTarget) {
    cam->crouchOffset += CROUCH_SPEED * deltaTime;
    if (cam->crouchOffset > crouchTarget)
      cam->crouchOffset = crouchTarget;
  } else if (cam->crouchOffset > crouchTarget) {
    cam->crouchOffset -= CROUCH_SPEED * deltaTime;
    if (cam->crouchOffset < crouchTarget)
      cam->crouchOffset = crouchTarget;
  }

  vec3 right;
  glm_vec3_cross(cam->front, cam->up, right);
  glm_vec3_normalize(right);

  vec3 move = {0.0f, 0.0f, 0.0f};

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    glm_vec3_muladds(cam->front, velocity, move);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    glm_vec3_muladds(cam->front, -velocity, move);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    glm_vec3_muladds(right, velocity, move);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    glm_vec3_muladds(right, -velocity, move);

  move[1] = 0.0f;

  camera_move_axis(cam, 0, move[0], scene, sceneCount);
  camera_move_axis(cam, 2, move[2], scene, sceneCount);

  cam->onGround = camera_check_ground(cam, scene, sceneCount);

  if (cam->onGround && cam->velocityY < 0.0f) {
    cam->velocityY = 0.0f;
  }

  if (cam->onGround && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    cam->velocityY = JUMP_FORCE;
    cam->onGround = 0;
  }

  cam->velocityY -= GRAVITY * deltaTime;

  vec3 verticalNewPos;
  glm_vec3_copy(cam->position, verticalNewPos);
  verticalNewPos[1] += cam->velocityY * deltaTime;

  if (!player_blocked_at(cam, verticalNewPos, scene, sceneCount)) {
    cam->position[1] = verticalNewPos[1];
  } else {
    cam->velocityY = 0.0f;
  }
}
