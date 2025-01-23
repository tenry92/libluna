#pragma once

#include <libluna/Vector.hpp>

namespace Luna {
  class Drawable2d {
    public:
    Drawable2d();
    ~Drawable2d();

    ///@{
    void setPosition(const Vector2f& position);
    Vector2f getPosition() const;
    ///@}

    /**
     * @name Priority
     *
     * Low values (negative) are drawn behind other drawables with higher values.
     */
    ///@{
    void setPriority(float priority);
    float getPriority() const;
    ///@}

    ///@{
    void setVisible(bool visible);
    bool isVisible() const;
    ///@}

    private:
    Vector2f mPosition;
    float mPriority{0};
    bool mVisible{true};
  };
} // namespace Luna
