#pragma once

#include <array>
#include <memory>

#include <libluna/Vector.hpp>

namespace Luna {
  class Matrix4x4 {
    public:
    Matrix4x4();
    Matrix4x4(const Matrix4x4 &other);
    Matrix4x4(const std::array<std::array<float, 4>, 4> &cols);
    ~Matrix4x4();
    bool operator==(const Matrix4x4 &other) const;
    Matrix4x4 &operator=(const Matrix4x4 &other);
    Matrix4x4 operator*(const Matrix4x4 &other) const;
    float &at(int row, int column) const;
    float *getValuePointer() const;

    Matrix4x4 translate(Vector3f vector) const;
    Matrix4x4 rotateX(float angle) const;
    Matrix4x4 rotateY(float angle) const;
    Matrix4x4 rotateZ(float angle) const;

    static Matrix4x4 identity();
    static Matrix4x4
    perspective(float fov, float aspect, float near, float far);

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
