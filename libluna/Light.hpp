#pragma once

#include <libluna/Color.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  struct AmbientLight {
    Color color{1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
  };

  struct PointLight {
    Color color{1.0f, 1.0f, 1.0f};
    Vector3f position;
  };
} // namespace Luna
