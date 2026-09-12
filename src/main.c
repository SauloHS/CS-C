#include "mesh.h"
#include "shader.h"
#include "window.h"
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>

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

  // Model matrix
  mat4 model;
  glm_mat4_identity(model);
  glm_translate(model, (vec3){0.0f, 0.0f, -3.0f});
  glm_rotate(model, glm_rad(30.0f), (vec3){0.0f, 1.0f, 0.0f});

  // View matrix
  mat4 view;
  glm_mat4_identity(view);
  glm_lookat((vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, -1.0f},
             (vec3){0.0f, 1.0f, 0.0f}, view);

  // Projection matrix
  mat4 projection;
  glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

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
