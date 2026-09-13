#include "texture.h"
#include <gl/gl.h>
#include <stb_image.h>
#include <stdio.h>

GLuint texture_create(const char *path) {
  int width, height, channels;
  stbi_set_flip_vertically_on_load(0);
  unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

  if (!data) {
    fprintf(stderr, "Failed to load texture: %s\n", path);
  }

  GLenum format;
  if (channels == 1)
    format = GL_RED;
  else if (channels == 3)
    format = GL_RGB;
  else if (channels == 4)
    format = GL_RGBA;
  else {
    fprintf(stderr, "Unsuported channel count (%d) in: %s\n", channels, path);
    stbi_image_free(data);
    return 0;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}
