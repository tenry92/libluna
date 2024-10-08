#include <libluna/Camera3d.hpp>

using namespace Luna;

Camera3d::Camera3d() : mRotation{Matrix4x4::identity()} {}

Camera3d::~Camera3d() = default;

void Camera3d::resetRotation() { mRotation = Matrix4x4::identity(); }

Vector3f Camera3d::getPosition() const { return mPosition; }

void Camera3d::setPosition(const Vector3f &position) {
  mPosition = position;
}

void Camera3d::rotateY(float angle) {
  mRotation = mRotation.rotateY(angle);
}

float Camera3d::getFov() const { return mFov; }

void Camera3d::setFov(float fov) { mFov = fov; }

std::tuple<float, float> Camera3d::getClipPlanes() const {
  return std::tuple<float, float>(mClipNear, mClipFar);
}

void Camera3d::setClipPlanes(float near, float far) {
  mClipNear = near;
  mClipFar = far;
}

Matrix4x4 Camera3d::getProjectionMatrix(float aspectRatio) const {
  return Matrix4x4::perspective(
      mFov, aspectRatio, mClipNear, mClipFar
  );
}

Matrix4x4 Camera3d::getViewMatrix() const {
  return mRotation * Matrix4x4::identity().translate(-mPosition);
}
