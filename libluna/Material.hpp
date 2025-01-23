#pragma once

#include <libluna/ImageLoader.hpp>
#include <libluna/Resource.hpp>
#include <libluna/Sprite.hpp>

namespace Luna {
  class Material {
    public:
    Material();
    ~Material();

    void setDiffuse(ImageLoader* image);
    ImageLoader* getDiffuse() const;

    void setNormal(ImageLoader* image);
    ImageLoader* getNormal() const;

    private:
    ImageLoader* mDiffuse{nullptr};
    ImageLoader* mNormal{nullptr};
  };
} // namespace Luna
