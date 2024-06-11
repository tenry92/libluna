#define USE_GLM

#include <array>
#include <cmath>
#include <iostream>

#ifdef USE_GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#include <libluna/Matrix.hpp>

using namespace Luna;

class Matrix4x4::impl {
  public:
#ifdef USE_GLM
  glm::mat4 mMatrix;
#else
  std::array<float, 4 * 4> mElements;
#endif
};

Matrix4x4::Matrix4x4() : mImpl{std::make_unique<impl>()} {}

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
#ifdef USE_GLM
  return mImpl->mMatrix == other.mImpl->mMatrix;
#else
  return mImpl->mElements == other.mImpl->mElements;
#endif
}

Matrix4x4 &Matrix4x4::operator=(const Matrix4x4 &other) {
#ifdef USE_GLM
  mImpl->mMatrix = other.mImpl->mMatrix;
#else
  mImpl->mElements = other.mImpl->mElements;
#endif

  return *this;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &other) const {
  Matrix4x4 result;

#ifdef USE_GLM
  result.mImpl->mMatrix = mImpl->mMatrix * other.mImpl->mMatrix;
#else
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      float value = 0.0f;

      for (int i = 0; i < 4; ++i) {
        value += at(row, i) * other.at(i, col);
      }

      result.at(row, col) = value;
    }
  }
#endif

  return result;
}

float &Matrix4x4::at(int row, int column) const {
#ifdef USE_GLM
  return glm::value_ptr(mImpl->mMatrix)[row + column * 4];
#else
  return mImpl->mElements.at(row + column * 4);
#endif
}

float *Matrix4x4::getValuePointer() const {
#ifdef USE_GLM
  return glm::value_ptr(mImpl->mMatrix);
#else
  return &mImpl->mElements.at(0);
#endif
}

Matrix4x4 Matrix4x4::translate(Vector3f vector) const {
  Matrix4x4 matrix = *this;

#ifdef USE_GLM
  matrix.mImpl->mMatrix = glm::translate(
      matrix.mImpl->mMatrix, glm::vec3(vector.x(), vector.y(), vector.z())
  );
#else
  matrix.at(0, 3) += vector.x();
  matrix.at(1, 3) += vector.y();
  matrix.at(2, 3) += vector.z();
#endif

  return matrix;
}

Matrix4x4 Matrix4x4::rotateX(float angle) const {
#ifdef USE_GLM
  Matrix4x4 matrix = *this;
  matrix.mImpl->mMatrix =
      glm::rotate(matrix.mImpl->mMatrix, angle, glm::vec3(1.0f, 0.0f, 0.0f));

  return matrix;
#else
  auto rotationMatrix = identity();

  rotationMatrix.at(1, 1) = std::cos(angle);
  rotationMatrix.at(1, 2) = -std::sin(angle);
  rotationMatrix.at(2, 1) = std::sin(angle);
  rotationMatrix.at(2, 2) = std::cos(angle);

  return *this * rotationMatrix;
#endif
}

Matrix4x4 Matrix4x4::rotateY(float angle) const {
#ifdef USE_GLM
  Matrix4x4 matrix = *this;
  matrix.mImpl->mMatrix =
      glm::rotate(matrix.mImpl->mMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));

  return matrix;
#else
  auto rotationMatrix = identity();

  rotationMatrix.at(0, 0) = std::cos(angle);
  rotationMatrix.at(0, 2) = std::sin(angle);
  rotationMatrix.at(2, 0) = -std::sin(angle);
  rotationMatrix.at(2, 2) = std::cos(angle);

  return *this * rotationMatrix;
#endif
}

Matrix4x4 Matrix4x4::rotateZ(float angle) const {
#ifdef USE_GLM
  Matrix4x4 matrix = *this;
  matrix.mImpl->mMatrix =
      glm::rotate(matrix.mImpl->mMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));

  return matrix;
#else
  auto rotationMatrix = identity();

  rotationMatrix.at(0, 0) = std::cos(angle);
  rotationMatrix.at(0, 1) = -std::sin(angle);
  rotationMatrix.at(1, 0) = std::sin(angle);
  rotationMatrix.at(1, 1) = std::cos(angle);

  return *this * rotationMatrix;
#endif
}

Matrix4x4 Matrix4x4::identity() {
  Matrix4x4 matrix;

#ifdef USE_GLM
  matrix.mImpl->mMatrix = glm::mat4(1.0f);
#else
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      matrix.at(row, col) = row == col;
    }
  }
#endif

  return matrix;
}

Matrix4x4
Matrix4x4::perspective(float fov, float aspect, float near, float far) {
  Matrix4x4 matrix;

#ifdef USE_GLM
  matrix.mImpl->mMatrix =
      glm::perspective(glm::radians(fov), aspect, near, far);
#else
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      matrix.at(row, col) = 0.0f;
    }
  }

  matrix.at(0, 0) = 1.0f / (aspect * std::tan(fov / 2.0f));
  matrix.at(1, 1) = 1.0f / std::tan(fov / 2.0f);
  matrix.at(2, 2) = -(far + near) / (far - near);
  matrix.at(2, 3) = (2 * far * near) / (far - near);
  matrix.at(3, 2) = -1.0f;
#endif

  return matrix;
}
