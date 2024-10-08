#pragma once

#include <list>
#include <variant>

#include <libluna/Light.hpp>
#include <libluna/Model.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/Text.hpp>
#include <libluna/TextureCache.hpp>

namespace Luna {
  /**
   * @brief Container for all visible items on a canvas.
   *
   * A stage contains 2D and 3D objects simultaneously.
   *
   * 2D objects are
   * positioned on the screen using 2D pixel coordinates, originating form the
   * top-left corner.
   *
   * @image html coordinate-system-2d.svg "2D Coordinate System in libluna"
   *
   * 3D objects are positioned in a 3D world, coordinates usually in meters.
   * In 3D, the x-axis points to the right, the y-axis points up and the z-axis
   * points towards the viewer (backwards).
   *
   * @image html coordinate-system-3d.svg "3D Coordinate System in libluna"
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

    TextureCache *getTextureCache();
    void updateTextureCache();

    private:
    std::list<Drawable2d> mDrawables2d;
    std::list<Drawable3d> mDrawables3d;
    AmbientLight mAmbientLight;
    std::list<std::shared_ptr<PointLight>> mPointLights;
    TextureCache mTextureCache;
  };
} // namespace Luna
