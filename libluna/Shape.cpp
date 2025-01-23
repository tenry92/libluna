#include <libluna/Shape.hpp>

#include <cmath> // cos, sin

#include <libluna/Math.hpp>

using Luna::Math::kPi;

namespace Luna {
  Shape::Shape() = default;
  Shape::~Shape() = default;

  Shape Shape::createCircle(float radius, int segments) {
    Shape shape;

    for (int i = 0; i < segments; i++) {
      float angle = static_cast<float>(2.0 * kPi * i / segments);
      shape.mVertices.push_back(
        Vector2f(radius * std::cos(angle), radius * std::sin(angle))
      );
    }

    return shape;
  }

  std::vector<Vector2f> &Shape::getVertices() { return mVertices; }
} // namespace Luna
