#pragma once

#include <memory>

#include <libluna/Image.hpp>
#include <libluna/ResourceRef.hpp>

namespace Luna {
  class Material {
    public:
    Material();
    ~Material();
    Material(const Material &other);
    Material &operator=(const Material &other);

    void setDiffuse(std::shared_ptr<ResourceRef<Image>> image);
    std::shared_ptr<ResourceRef<Image>> getDiffuse() const;

    void setNormal(std::shared_ptr<ResourceRef<Image>> image);
    std::shared_ptr<ResourceRef<Image>> getNormal() const;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
