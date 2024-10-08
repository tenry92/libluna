#pragma once

#include <tuple>

#include <libluna/Matrix.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Camera3d {
    public:
    Camera3d();
    ~Camera3d();

    Vector3f getPosition() const;
    void setPosition(const Vector3f &position);

    void rotateY(float angle);
    void resetRotation();

    float getFov() const;
    void setFov(float fov);

    std::tuple<float, float> getClipPlanes() const;
    void setClipPlanes(float near, float far);

    Matrix4x4 getProjectionMatrix(float aspectRatio) const;
    Matrix4x4 getViewMatrix() const;

    private:
    Vector3f mPosition;
    float mFov{45.0f};
    float mClipNear{0.1f};
    float mClipFar{1000.0f};
    Matrix4x4 mRotation;
  };
} // namespace Luna
