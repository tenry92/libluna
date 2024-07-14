#pragma once

#include <list>
#include <memory>

#include <libluna/Light.hpp>
#include <libluna/Model.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/Text.hpp>

namespace Luna {
  class Stage {
    public:
    Stage();
    ~Stage();

    SpritePtr makeSprite();
    const std::list<SpritePtr> &getSprites() const;

    TextPtr makeText();
    const std::list<TextPtr> &getTexts() const;

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
