#pragma once

#include <glad/glad.h>

#define CHECK_GL(x)                                                            \
  {                                                                            \
    glGetError();                                                              \
    GLenum err;                                                                \
    x;                                                                         \
    while ((err = glGetError()) != GL_NO_ERROR)                                \
      logError("opengl error [" #x "]: {}", getGlErrorString(err));            \
  }

const char *getGlErrorString(GLenum err) {
  switch (err) {
  default:
    return "unknown";
  case GL_INVALID_ENUM:
    return "invalid enum";
  case GL_INVALID_VALUE:
    return "invalid value";
  case GL_INVALID_OPERATION:
    return "invalid operation";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "invalid framebuffer operation";
  case GL_OUT_OF_MEMORY:
    return "out of memory";
  }
}
