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
} // namespace Luna
