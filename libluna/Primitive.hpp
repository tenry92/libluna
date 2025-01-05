#pragma once

#include <libluna/Shape.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  /**
   * @brief A primitive shape.
   * 
   * @ingroup drawables2d
   */
  class Primitive {
    public:
    Primitive();
    ~Primitive();

    ///@{
    void setShape(Shape *shape);
    Shape *getShape() const;
    ///@}

    ///@{
    void setPosition(const Vector2f &position);
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

    private:
    Shape *mShape;
    Vector2f mPosition;
    float mPriority{0};
  };
}
