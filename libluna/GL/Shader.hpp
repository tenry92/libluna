#pragma once

#include <vector>

#include <glad/glad.h>

#include <libluna/GL/Uniform.hpp>
#include <libluna/InputStream.hpp>
#include <libluna/Logger.hpp>
#include <libluna/String.hpp>

namespace Luna::GL {
  static void checkShaderCompileStatus(GLuint shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(shader, 512, nullptr, infoLog);
      Luna::logError("opengl shader compilation error: {}", infoLog);
    }
  }

  static void checkProgramLinkStatus(GLuint program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
      glGetProgramInfoLog(program, 512, nullptr, infoLog);
      Luna::logError("opengl program link error: {}", infoLog);
    }
  }

  class Shader {
    public:
    inline Shader() {}
    inline Shader(const char *vertexSource, const char *fragmentSource) {
      GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
      glShaderSource(vertexShader, 1, &vertexSource, nullptr);
      glCompileShader(vertexShader);
      checkShaderCompileStatus(vertexShader);

      GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
      glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
      glCompileShader(fragmentShader);
      checkShaderCompileStatus(fragmentShader);

      mShaderProgram = glCreateProgram();
      glAttachShader(mShaderProgram, vertexShader);
      glAttachShader(mShaderProgram, fragmentShader);
      glBindFragDataLocation(mShaderProgram, 0, "fColor");
      glLinkProgram(mShaderProgram);
      checkProgramLinkStatus(mShaderProgram);

      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);
    }

    Shader(const Shader &other) = delete;

    inline Shader &operator=(Shader &&other) {
      mShaderProgram = other.mShaderProgram;
      other.mShaderProgram = 0;
      return *this;
    }

    inline ~Shader() {
      if (mShaderProgram) {
        glDeleteProgram(mShaderProgram);
        mShaderProgram = 0;
      }
    }

    inline void use() const { glUseProgram(mShaderProgram); }

    inline Uniform getUniform(const String &name) {
      return Uniform(mShaderProgram, name);
    }

    inline Uniform getUniform(const String &name, int count) {
      return Uniform(mShaderProgram, name, count);
    }

    private:
    GLuint mShaderProgram{0};
  };
} // namespace Luna::GL
