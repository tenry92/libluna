#include <libluna/Primitive.hpp>

namespace Luna {
  Primitive::Primitive() = default;

  Primitive::~Primitive() = default;

  void Primitive::setShape(Shape *shape) {
    mShape = shape;
  }

  Shape *Primitive::getShape() const {
    return mShape;
  }

  void Primitive::setPosition(const Vector2f &position) {
    mPosition = position;
  }

  Vector2f Primitive::getPosition() const {
    return mPosition;
  }

  void Primitive::setPriority(float priority) {
    mPriority = priority;
  }

  float Primitive::getPriority() const {
    return mPriority;
  }
} // namespace Luna
