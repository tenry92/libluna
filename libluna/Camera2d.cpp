#include <libluna/Camera2d.hpp>

using namespace Luna;

class Camera2d::impl {
  public:
  Vector2f mPosition;
};

Camera2d::Camera2d() : mImpl{std::make_unique<impl>()} {}

Camera2d::Camera2d(const Camera2d &other) : mImpl{std::make_unique<impl>()} {
  *mImpl = *other.mImpl;
}

Camera2d::~Camera2d() = default;

Camera2d &Camera2d::operator=(const Camera2d &other) {
  *mImpl = *other.mImpl;

  return *this;
}

Vector2f Camera2d::getPosition() const { return mImpl->mPosition; }

void Camera2d::setPosition(const Vector2f &position) {
  mImpl->mPosition = position;
}
