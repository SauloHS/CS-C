#include <glad/glad.h>

#include "camera.h"
#include "mesh.h"
#include "shader.h"
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

  GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
  GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
  GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

  Mesh mesh = cube_mesh_create();
  Camera camera = camera_create();
  glfwSetWindowUserPointer(window, &camera);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  // Model matrix
  mat4 model;
  glm_mat4_identity(model);
  glm_translate(model, (vec3){0.0f, 0.0f, -3.0f});
  glm_rotate(model, glm_rad(30.0f), (vec3){0.0f, 1.0f, 0.0f});

  // Projection matrix
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

    camera_process_keyboard(&camera, window, deltaTime);

    mat4 view;
    camera_get_view_matrix(&camera, view);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float *)projection);

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.count, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  mesh_delete(mesh);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
