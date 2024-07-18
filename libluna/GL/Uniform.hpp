#pragma once

#include <vector>

#include <glad/glad.h>

#include <libluna/Color.hpp>
#include <libluna/Logger.hpp>
#include <libluna/Matrix.hpp>
#include <libluna/String.hpp>
#include <libluna/Vector.hpp>

namespace Luna::GL {
  class Uniform {
    public:
    inline Uniform() {}

    inline Uniform(
        [[maybe_unused]] GLuint program, [[maybe_unused]] const String &name
    ) {
      mLocation = glGetUniformLocation(program, name.c_str());

      if (mLocation == -1) {
        Luna::logWarn("unknown uniform \"{}\"", name);
      }
    }

    inline Uniform(GLuint program, const String &name, int count) {
      mArray.reserve(count);

      for (int i = 0; i < count; ++i) {
        mArray.emplace_back(Uniform(program, name.format(i).c_str()));
      }
    }

    inline Uniform &operator[](std::size_t index) { return mArray.at(index); }

    inline int operator=(int value) const {
      glUniform1i(mLocation, value);
      return value;
    }

    inline float operator=(float value) const {
      glUniform1f(mLocation, value);
      return value;
    }

    inline Luna::Vector2f operator=(Luna::Vector2f value) const {
      glUniform2f(mLocation, value.x(), value.y());
      return value;
    }

    inline Luna::Vector3f operator=(Luna::Vector3f value) const {
      glUniform3f(mLocation, value.x(), value.y(), value.z());
      return value;
    }

    inline Luna::ColorRgb operator=(Luna::ColorRgb value) const {
      glUniform3f(mLocation, value.red, value.green, value.blue);
      return value;
    }

    inline Luna::Matrix4x4 operator=(const Luna::Matrix4x4 &value) const {
      glUniformMatrix4fv(mLocation, 1, GL_FALSE, value.getValuePointer());
      return value;
    }

    private:
    int mLocation;
    std::vector<Uniform> mArray;
  };
} // namespace Luna::GL
