#pragma once

#include <libluna/Color.hpp>
#include <libluna/Drawable2d.hpp>
#include <libluna/Texture.hpp>
#include <libluna/Palette.hpp>
#include <libluna/String.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Sprite;

  /**
   * @defgroup drawables2d 2D Drawables
   *
   * @brief 2D drawables that can be created on a stage.
   */
  /**
   * @brief A 2D sprite.
   *
   * @ingroup drawables2d canvas
   */
  class Sprite final : public Drawable2d {
    public:
    Sprite();
    ~Sprite();

    ///@{
    void setTexture(int id);
    int getTexture() const;
    ///@}

    private:
    int mTextureId{0};
  };
} // namespace Luna
