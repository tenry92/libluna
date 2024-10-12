#pragma once

#include <libluna/Image.hpp>
#include <libluna/Resource.hpp>

namespace Luna {
  class Material {
    public:
    Material();
    ~Material();

    void setDiffuse(Image *image);
    Image *getDiffuse() const;

    void setNormal(Image *image);
    Image *getNormal() const;

    private:
    Image *mDiffuse{nullptr};
    Image *mNormal{nullptr};
  };
} // namespace Luna
