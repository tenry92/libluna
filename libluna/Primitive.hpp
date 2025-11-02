#pragma once

#include <libluna/Drawable2d.hpp>
#include <libluna/Shape.hpp>

namespace Luna {
  /**
   * @brief A primitive shape.
   *
   * @ingroup drawables2d canvas
   */
  class Primitive final : public Drawable2d {
    public:
    Primitive();
    ~Primitive();

    ///@{
    void setShape(Shape* shape);
    Shape* getShape() const;
    ///@}

    private:
    Shape* mShape;
  };
} // namespace Luna
