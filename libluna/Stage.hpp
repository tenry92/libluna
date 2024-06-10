#pragma once

#include <list>
#include <memory>

#include <libluna/Light.hpp>
#include <libluna/Model.hpp>
#include <libluna/Sprite.hpp>

namespace Luna {
  class Stage {
    public:
    Stage();
    ~Stage();

    std::shared_ptr<Sprite> makeSprite();
    const std::list<std::shared_ptr<Sprite>> &getSprites() const;

    std::shared_ptr<Model> makeModel();
    const std::list<std::shared_ptr<Model>> &getModels() const;

    void setAmbientLight(const AmbientLight &ambientLight);
    AmbientLight getAmbientLight() const;

    std::shared_ptr<PointLight> makePointLight();
    const std::list<std::shared_ptr<PointLight>> &getPointLights() const;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
