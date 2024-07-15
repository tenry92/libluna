#pragma once

#include <list>
#include <memory>
#include <variant>

#include <libluna/Light.hpp>
#include <libluna/Model.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/Text.hpp>

namespace Luna {
  /**
   * @brief Container for all visible items on a canvas.
   *
   * A stage can be assigned to multiple canvases. For example, if each canvas
   * uses a different renderer, the same stage can be viewed and tested from
   * different canvases.
   */
  class Stage {
    public:
    using Drawable2d = std::variant<SpritePtr, TextPtr>;
    using Drawable3d = ModelPtr;
    Stage();
    ~Stage();

    void add(SpritePtr sprite);
    void add(TextPtr text);
    void add(ModelPtr model);
    void remove(SpritePtr sprite);
    void remove(TextPtr text);
    void remove(ModelPtr model);

    const std::list<Drawable2d> &getDrawables2d() const;
    const std::list<Drawable3d> &getDrawables3d() const;

    void setAmbientLight(const AmbientLight &ambientLight);
    AmbientLight getAmbientLight() const;

    std::shared_ptr<PointLight> makePointLight();
    const std::list<std::shared_ptr<PointLight>> &getPointLights() const;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
