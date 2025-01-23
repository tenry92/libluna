#pragma once

#include <array>
#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libluna/Matrix.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Matrix4x4 {
    public:
    Matrix4x4();
    Matrix4x4(const Matrix4x4& other);
    Matrix4x4(const std::array<std::array<float, 4>, 4>& cols);
    ~Matrix4x4();
    bool operator==(const Matrix4x4& other) const;
    Matrix4x4& operator=(const Matrix4x4& other);
    Matrix4x4 operator*(const Matrix4x4& other) const;
    float& at(int row, int column);
    float at(int row, int column) const;
    const float* getValuePointer() const;

    Matrix4x4 translate(Vector3f vector) const;
    Matrix4x4 rotateX(float angle) const;
    Matrix4x4 rotateY(float angle) const;
    Matrix4x4 rotateZ(float angle) const;

    static Matrix4x4 identity();
    static Matrix4x4
    perspective(float fov, float aspect, float near, float far);

    private:
    glm::mat4 mMatrix;
  };
} // namespace Luna
