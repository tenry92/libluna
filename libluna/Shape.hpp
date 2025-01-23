#pragma once

#include <vector>

#include <libluna/Vector.hpp>

namespace Luna {
  class Shape {
    public:
    Shape();
    ~Shape();

    static Shape createCircle(float radius, int segments = 32);

    std::vector<Vector2f>& getVertices();

    private:
    std::vector<Vector2f> mVertices;
  };
} // namespace Luna
