#include <libluna/Camera3d.hpp>

using namespace Luna;

class Camera3d::impl {
  public:
  Vector3f mPosition;
  float mFov{45.0f};
  float mClipNear{0.1f};
  float mClipFar{1000.0f};
  Matrix4x4 mRotation;
};

Camera3d::Camera3d() : mImpl{std::make_unique<impl>()} {
  mImpl->mRotation = Matrix4x4::identity();
}

Camera3d::Camera3d(const Camera3d &other) : Camera3d() { *this = other; }

Camera3d &Camera3d::operator=(const Camera3d &other) {
  *mImpl = *other.mImpl;

  return *this;
}

Camera3d::~Camera3d() = default;

void Camera3d::resetRotation() { mImpl->mRotation = Matrix4x4::identity(); }

Vector3f Camera3d::getPosition() const { return mImpl->mPosition; }

void Camera3d::setPosition(const Vector3f &position) {
  mImpl->mPosition = position;
}

void Camera3d::rotateY(float angle) {
  mImpl->mRotation = mImpl->mRotation.rotateY(angle);
}

float Camera3d::getFov() const { return mImpl->mFov; }

void Camera3d::setFov(float fov) { mImpl->mFov = fov; }

std::tuple<float, float> Camera3d::getClipPlanes() const {
  return std::tuple<float, float>(mImpl->mClipNear, mImpl->mClipFar);
}

void Camera3d::setClipPlanes(float near, float far) {
  mImpl->mClipNear = near;
  mImpl->mClipFar = far;
}

Matrix4x4 Camera3d::getProjectionMatrix(float aspectRatio) const {
  return Matrix4x4::perspective(
      mImpl->mFov, aspectRatio, mImpl->mClipNear, mImpl->mClipFar
  );
}

Matrix4x4 Camera3d::getViewMatrix() const {
  return mImpl->mRotation * Matrix4x4::identity().translate(-mImpl->mPosition);
}
