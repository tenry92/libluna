#pragma once

#include <memory>
#include <tuple>

#include <libluna/Matrix.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Camera3d {
    public:
    Camera3d();
    Camera3d(const Camera3d &other);
    Camera3d &operator=(const Camera3d &other);
    ~Camera3d();

    Vector3f getPosition() const;
    void setPosition(const Vector3f &position);

    void rotateY(float angle);

    float getFov() const;
    void setFov(float fov);

    std::tuple<float, float> getClipPlanes() const;
    void setClipPlanes(float near, float far);

    Matrix4x4 getProjectionMatrix(float aspectRatio) const;
    Matrix4x4 getViewMatrix() const;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
