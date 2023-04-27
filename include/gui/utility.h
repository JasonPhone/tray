#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
GLuint create_texture(uint8_t* image, int image_w, int image_h) {
  GLuint texture_id;
  glGenTextures(1, &texture_id);

  int img_w = image_w, img_h = image_h;
  unsigned char* data = image;
  if (data) {
    GLenum format = GL_RGB;
    // For sanity check.
    GLenum warp = GL_REPEAT;
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, img_w, img_h, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  return texture_id;
}