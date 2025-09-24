#pragma once

#include <libluna/Color.hpp>
#include <libluna/Drawable2d.hpp>
#include <libluna/Image.hpp>
#include <libluna/ImageLoader.hpp>
#include <libluna/Palette.hpp>
#include <libluna/Resource.hpp>
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
   * @ingroup drawables2d
   */
  class Sprite final : public Drawable2d {
    public:
    Sprite();
    ~Sprite();

    ///@{
    void setImageLoader(ImageLoader* loader);
    ImageLoader* getImageLoader() const;
    ///@}

    ///@{
    void setPalette(ResourcePtr<Palette> palette);
    ResourcePtr<Palette> getPalette() const;
    ///@}

    private:
    ImageLoader* mImageLoader;
    std::shared_ptr<Resource<Palette>> mPalette;
  };
} // namespace Luna
