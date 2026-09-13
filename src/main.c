#include <glad/glad.h>

#include "camera.h"
#include "collision.h"
#include "mesh.h"
#include "scene.h"
#include "shader.h"
#include "texture.h"
#include "window.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
  camera_process_mouse(camera, xpos, ypos);
}

int main(void) {
  GLFWwindow *window = window_create();
  if (!window) {
    return 1;
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "Failed to initialize GLAD\n");
    return 1;
  }

  glEnable(GL_DEPTH_TEST);

  GLuint shaderProgram =
      shader_program_create("shaders/unlit.vert", "shaders/unlit.frag");
  GLuint cubeTexture = texture_create("textures/brick.png");

  GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
  GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
  GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
  SceneObject objects[] = {
      {.mesh = cube_mesh_create(),
       .position = {0.0f, 0.0f, -3.0f},
       .rotationY = 30.0f},
      {.mesh = floor_mesh_create(),
       .position = {0.0f, -10.0f, 0.0f},
       .rotationY = 0.0f},
  };
  int objectCount = sizeof(objects) / sizeof(SceneObject);

  AABB sceneBounds[objectCount];
  for (int i = 0; i < objectCount; i++) {
    sceneBounds[i] = objects[i].mesh.bounds;
  }
  int sceneCount = objectCount;

  Camera camera = camera_create();
  glfwSetWindowUserPointer(window, &camera);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  mat4 projection;
  glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    camera_process_keyboard(&camera, window, deltaTime, sceneBounds,
                            sceneCount);

    mat4 view;
    camera_get_view_matrix(&camera, view);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float *)projection);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);

    for (int i = 0; i < objectCount; i++) {
      mat4 objModel;
      scene_object_get_model(&objects[i], objModel);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)objModel);

      glBindVertexArray(objects[i].mesh.VAO);
      glDrawElements(GL_TRIANGLES, objects[i].mesh.count, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  for (int i = 0; i < objectCount; i++) {
    mesh_delete(objects[i].mesh);
  }
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
