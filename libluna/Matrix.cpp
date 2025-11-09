#define USE_GLM

#include <libluna/config.h>

#include <array>
#include <cmath>
#include <iostream>

#ifdef LUNA_GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#include <libluna/Matrix.hpp>

using namespace Luna;

Matrix4x4::Matrix4x4() = default;

Matrix4x4::Matrix4x4(const Matrix4x4& other) : Matrix4x4() { *this = other; }

Matrix4x4::Matrix4x4(const std::array<std::array<float, 4>, 4>& cols)
    : Matrix4x4() {
#ifdef LUNA_GLM
  for (int col = 0; col < 4; ++col) {
    for (int row = 0; row < 4; ++row) {
      this->at(row, col) = cols.at(col).at(row);
    }
  }
#else
  mMatrix = cols;
#endif
}

Matrix4x4::~Matrix4x4() = default;

bool Matrix4x4::operator==(const Matrix4x4& other) const {
  return mMatrix == other.mMatrix;
}

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& other) {
  mMatrix = other.mMatrix;

  return *this;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
  Matrix4x4 result;

#ifdef LUNA_GLM
  result.mMatrix = mMatrix * other.mMatrix;
#else
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      result.at(row, col) = 0.0f;
      for (int k = 0; k < 4; ++k) {
        result.at(row, col) += this->at(row, k) * other.at(k, col);
      }
    }
  }
#endif

  return result;
}

float& Matrix4x4::at(int row, int column) {
#ifdef LUNA_GLM
  return glm::value_ptr(mMatrix)[row + column * 4];
#else
  return mMatrix.at(column).at(row);
#endif
}

float Matrix4x4::at(int row, int column) const {
#ifdef LUNA_GLM
  return glm::value_ptr(mMatrix)[row + column * 4];
#else
  return mMatrix.at(column).at(row);
#endif
}

const float* Matrix4x4::getValuePointer() const {
#ifdef LUNA_GLM
  return glm::value_ptr(mMatrix);
#else
  return &mMatrix[0][0];
#endif
}

Matrix4x4 Matrix4x4::translate(Vector3f vector) const {
  Matrix4x4 matrix = *this;

#ifdef LUNA_GLM
  matrix.mMatrix =
    glm::translate(matrix.mMatrix, glm::vec3(vector.x, vector.y, vector.z));
#else
  matrix.at(0, 3) += vector.x;
  matrix.at(1, 3) += vector.y;
  matrix.at(2, 3) += vector.z;
#endif

  return matrix;
}

Matrix4x4 Matrix4x4::rotateX(float angle) const {
  Matrix4x4 matrix = *this;

#ifdef LUNA_GLM
  matrix.mMatrix =
    glm::rotate(matrix.mMatrix, angle, glm::vec3(1.0f, 0.0f, 0.0f));
#else
  float c = std::cos(angle);
  float s = std::sin(angle);

  matrix.at(1, 1) = c;
  matrix.at(1, 2) = -s;
  matrix.at(2, 1) = s;
  matrix.at(2, 2) = c;
#endif

  return matrix;
}

Matrix4x4 Matrix4x4::rotateY(float angle) const {
  Matrix4x4 matrix = *this;

#ifdef LUNA_GLM
  matrix.mMatrix =
    glm::rotate(matrix.mMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
#else
  float c = std::cos(angle);
  float s = std::sin(angle);

  matrix.at(0, 0) = c;
  matrix.at(0, 2) = s;
  matrix.at(2, 0) = -s;
  matrix.at(2, 2) = c;
#endif

  return matrix;
}

Matrix4x4 Matrix4x4::rotateZ(float angle) const {
  Matrix4x4 matrix = *this;

#ifdef LUNA_GLM
  matrix.mMatrix =
    glm::rotate(matrix.mMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
#else
  float c = std::cos(angle);
  float s = std::sin(angle);

  matrix.at(2, 0) = -s;
  matrix.at(2, 2) = c;
#endif

  return matrix;
}

Matrix4x4 Matrix4x4::identity() {
  Matrix4x4 matrix;

#ifdef LUNA_GLM
  matrix.mMatrix = glm::mat4(1.0f);
#else
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      matrix.at(i, j) = (i == j) ? 1.0f : 0.0f;
    }
  }
#endif

  return matrix;
}

Matrix4x4
Matrix4x4::perspective(float fov, float aspect, float near, float far) {
  Matrix4x4 matrix;

#ifdef LUNA_GLM
  matrix.mMatrix = glm::perspective(glm::radians(fov), aspect, near, far);
#else
  float f = 1.0f / std::tan(fov / 2.0f);
  matrix.at(0, 0) = f / aspect;
  matrix.at(1, 1) = f;
  matrix.at(2, 2) = (far + near) / (near - far);
  matrix.at(2, 3) = (2.0f * far * near) / (near - far);
  matrix.at(3, 2) = -1.0f;
  matrix.at(3, 3) = 0.0f;
#endif

  return matrix;
}
