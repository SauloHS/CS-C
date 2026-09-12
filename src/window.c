#include "window.h"
#include <stdio.h>

GLFWwindow *window_create(void) {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return NULL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "CS2", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to initialize window\n");
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);
  return window;
}
