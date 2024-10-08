#pragma once

#include <libluna/Vector.hpp>

namespace Luna {
  class Camera2d {
    public:
    Camera2d();
    Camera2d(const Camera2d &other);
    ~Camera2d();

    Camera2d &operator=(const Camera2d &other);

    Vector2f getPosition() const;
    void setPosition(const Vector2f &position);

    private:
    Vector2f mPosition;
  };
} // namespace Luna
