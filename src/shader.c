#include "shader.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

static GLuint compile_shader(GLenum type, const char *source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr, "Error at %s shader:\n%s\n",
            type == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
    return 0;
  }

  return shader;
}

GLuint shader_program_create(const char *vertex_path, const char *fragment_path) {
  char *vertexSource = read_file(vertex_path);
  char *fragmentSource = read_file(fragment_path);

  GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentSource);

  free(vertexSource);
  free(fragmentSource);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  GLint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    fprintf(stderr, "Error at shader program:\n%s\n", infoLog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}
