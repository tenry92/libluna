#define USE_GLM

#include <array>
#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libluna/Matrix.hpp>

using namespace Luna;

Matrix4x4::Matrix4x4() = default;

Matrix4x4::Matrix4x4(const Matrix4x4 &other) : Matrix4x4() { *this = other; }

Matrix4x4::Matrix4x4(const std::array<std::array<float, 4>, 4> &cols)
    : Matrix4x4() {
  for (int col = 0; col < 4; ++col) {
    for (int row = 0; row < 4; ++row) {
      this->at(row, col) = cols.at(col).at(row);
    }
  }
}

Matrix4x4::~Matrix4x4() = default;

bool Matrix4x4::operator==(const Matrix4x4 &other) const {
  return mMatrix == other.mMatrix;
}

Matrix4x4 &Matrix4x4::operator=(const Matrix4x4 &other) {
  mMatrix = other.mMatrix;

  return *this;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &other) const {
  Matrix4x4 result;

  result.mMatrix = mMatrix * other.mMatrix;

  return result;
}

float &Matrix4x4::at(int row, int column) {
  return glm::value_ptr(mMatrix)[row + column * 4];
}

float Matrix4x4::at(int row, int column) const {
  return glm::value_ptr(mMatrix)[row + column * 4];
}

const float *Matrix4x4::getValuePointer() const {
  return glm::value_ptr(mMatrix);
}

Matrix4x4 Matrix4x4::translate(Vector3f vector) const {
  Matrix4x4 matrix = *this;

  matrix.mMatrix =
      glm::translate(matrix.mMatrix, glm::vec3(vector.x, vector.y, vector.z));

  return matrix;
}

Matrix4x4 Matrix4x4::rotateX(float angle) const {
  Matrix4x4 matrix = *this;
  matrix.mMatrix =
      glm::rotate(matrix.mMatrix, angle, glm::vec3(1.0f, 0.0f, 0.0f));

  return matrix;
}

Matrix4x4 Matrix4x4::rotateY(float angle) const {
  Matrix4x4 matrix = *this;
  matrix.mMatrix =
      glm::rotate(matrix.mMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));

  return matrix;
}

Matrix4x4 Matrix4x4::rotateZ(float angle) const {
  Matrix4x4 matrix = *this;
  matrix.mMatrix =
      glm::rotate(matrix.mMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));

  return matrix;
}

Matrix4x4 Matrix4x4::identity() {
  Matrix4x4 matrix;

  matrix.mMatrix = glm::mat4(1.0f);

  return matrix;
}

Matrix4x4
Matrix4x4::perspective(float fov, float aspect, float near, float far) {
  Matrix4x4 matrix;

  matrix.mMatrix = glm::perspective(glm::radians(fov), aspect, near, far);

  return matrix;
}
