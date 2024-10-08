#pragma once

#include <libluna/Image.hpp>
#include <libluna/Resource.hpp>

namespace Luna {
  class Material {
    public:
    Material();
    ~Material();

    void setDiffuse(ImageResPtr image);
    ImageResPtr getDiffuse() const;

    void setNormal(ImageResPtr image);
    ImageResPtr getNormal() const;

    private:
    ImageResPtr mDiffuse;
    ImageResPtr mNormal;
  };
} // namespace Luna
