#include <libluna/Drawable2d.hpp>

namespace Luna {
  Drawable2d::Drawable2d() = default;
  Drawable2d::~Drawable2d() = default;

  void Drawable2d::setPosition(const Vector2f& position) {
    mPosition = position;
  }

  Vector2f Drawable2d::getPosition() const { return mPosition; }

  void Drawable2d::setPriority(float priority) { mPriority = priority; }

  float Drawable2d::getPriority() const { return mPriority; }

  void Drawable2d::setVisible(bool visible) { mVisible = visible; }

  bool Drawable2d::isVisible() const { return mVisible; }
} // namespace Luna
