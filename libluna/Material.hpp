#pragma once

#include <memory>

#include <libluna/Image.hpp>
#include <libluna/Resource.hpp>

namespace Luna {
  class Material {
    public:
    Material();
    ~Material();
    Material(const Material &other);
    Material &operator=(const Material &other);

    void setDiffuse(ImageResPtr image);
    ImageResPtr getDiffuse() const;

    void setNormal(ImageResPtr image);
    ImageResPtr getNormal() const;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
