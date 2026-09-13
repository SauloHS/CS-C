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
#include <stdlib.h>

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

  SceneObject *objects = NULL;
  int objectCount = 0;
  if (!scene_load_gltf("assets/dust.gltf", &objects, &objectCount)) {
    return 1;
  }

  CollisionMesh *collision = malloc(objectCount * sizeof(CollisionMesh));
  for (int i = 0; i < objectCount; i++) {
    collision[i].tris = objects[i].tris;
    collision[i].triCount = objects[i].triCount;
    collision[i].bounds = objects[i].mesh.bounds;
  }

  Camera camera = camera_create();
  glfwSetWindowUserPointer(window, &camera);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  mat4 projection;
  glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f, projection);

  GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
  GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
  GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

  float deltaTime = 0.0f;
  float lastFrame = (float)glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    if (deltaTime > 0.05f) {
      deltaTime = 0.05f;
    }

    camera_process_keyboard(&camera, window, deltaTime, collision,
                            objectCount);

    mat4 view;
    camera_get_view_matrix(&camera, view);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float *)projection);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    glActiveTexture(GL_TEXTURE0);

    for (int i = 0; i < objectCount; i++) {
      mat4 objModel;
      scene_object_get_model(&objects[i], objModel);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)objModel);

      glBindTexture(GL_TEXTURE_2D, objects[i].mesh.texture);
      glBindVertexArray(objects[i].mesh.VAO);
      glDrawElements(GL_TRIANGLES, objects[i].mesh.count, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  scene_delete(objects, objectCount);
  free(collision);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
