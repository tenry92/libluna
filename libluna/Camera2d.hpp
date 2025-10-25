#pragma once

#include <libluna/Vector.hpp>

namespace Luna {
  class Stage;

  class Camera2d {
    public:
    Camera2d();
    Camera2d(const Camera2d& other);
    ~Camera2d();

    Camera2d& operator=(const Camera2d& other);

    Vector2f getPosition() const;
    void setPosition(const Vector2f& position);

    void setStage(Stage* stage);
    Stage* getStage() const;

    private:
    Vector2f mPosition;
    Stage* mStage{nullptr};
  };
} // namespace Luna
