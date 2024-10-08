#include <libluna/Camera2d.hpp>

using namespace Luna;

Camera2d::Camera2d() = default;

Camera2d::Camera2d(const Camera2d &other) : mPosition{other.mPosition} {}

Camera2d::~Camera2d() = default;

Camera2d &Camera2d::operator=(const Camera2d &other) {
  mPosition = other.mPosition;

  return *this;
}

Vector2f Camera2d::getPosition() const { return mPosition; }

void Camera2d::setPosition(const Vector2f &position) {
  mPosition = position;
}
