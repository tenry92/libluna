#pragma once

#include <libluna/Color.hpp>
#include <libluna/Image.hpp>
#include <libluna/ImageLoader.hpp>
#include <libluna/Palette.hpp>
#include <libluna/Resource.hpp>
#include <libluna/String.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Sprite;
  using SpritePtr = std::shared_ptr<Sprite>;

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
  class Sprite final {
    public:
    Sprite();
    ~Sprite();

    ///@{
    void setImageLoader(ImageLoader *loader);
    ImageLoader *getImageLoader() const;
    ///@}

    ///@{
    void setPalette(ResourcePtr<Palette> palette);
    ResourcePtr<Palette> getPalette() const;
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
    ImageLoader *mImageLoader;
    std::shared_ptr<Resource<Palette>> mPalette;
    Vector2f mPosition;
    float mPriority{0};
  };
} // namespace Luna
