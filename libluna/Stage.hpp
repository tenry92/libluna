#pragma once

#include <forward_list>
#include <list>
#include <variant>

#include <libluna/Light.hpp>
#include <libluna/Model.hpp>
#include <libluna/Pool.hpp>
#include <libluna/Primitive.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/Text.hpp>
#include <libluna/Tilemap.hpp>

namespace Luna {
  /**
   * @brief Container for all visible items on a canvas.
   *
   * A stage contains 2D and 3D objects simultaneously.
   *
   * ```cpp
   * class GameApp : public Luna::Application {
   *   public:
   *   using Application::Application;
   *
   *   protected:
   *   void init() override final {
   *     // first, create a canvas where the stage will be rendered
   *     mCanvas = this->makeCanvas({800, 600});
   *     mCanvas->setVideoDriver(this->getDefaultVideoDriver());
   *
   *     // then, assign our stage to the canvas
   *     mCanvas->setStage(&mStage);
   *
   *     // and create stuff to draw
   *     Luna::Sprite* sprite = mStage.createSprite();
   *     sprite->setImageLoader(YourImageLoader("player.png"));
   *     sprite->setPosition({400, 300});
   *
   *     // sprite will automatically be rendered on the canvas
   *   }
   *
   *   void update(float deltaTime) override final {}
   *
   *   private:
   *   Luna::Canvas* mCanvas;
   *   Luna::Stage mStage;
   * };
   * ```
   *
   * The following table shows the coordinate systems used in libluna:
   *
   * | Coordinate System | Origin | X-Axis | Y-Axis | Z-Axis | Unit | Image |
   * |-------------------|--------|--------|--------|--------|------|-------|
   * | 2D                | Top-left corner | Right | Down | Higher values are drawn above lower values | Pixels | @image html coordinate-system-2d.svg "2D Coordinate System in libluna" |
   * | 3D                | Center of the world | Right | Up | Backwards | Meters | @image html coordinate-system-3d.svg "3D Coordinate System in libluna" |
   *
   * A stage can be assigned to multiple canvases. For example, if each canvas
   * uses a different renderer, the same stage can be viewed and tested from
   * different canvases.
   */
  class Stage {
    public:
    using Drawable2dVariant = std::variant<Sprite, Primitive, Text, Tilemap>;
    using Drawable3d = Model*;
    Stage();
    ~Stage();

    Sprite* allocSprite();
    void freeSprite(Sprite* sprite);

    Primitive* allocPrimitive();
    void freePrimitive(Primitive* primitive);

    Text* allocText();
    void freeText(Text* text);

    Tilemap* allocTilemap();
    void freeTilemap(Tilemap* tilemap);

    Model* allocModel();
    void freeModel(Model* model);

    const Pool<Drawable2dVariant, 64>& getDrawables2d() const;
    const std::forward_list<const Drawable2dVariant*> getSortedDrawables2d() const;
    const std::list<Drawable3d>& getDrawables3d() const;

    void setAmbientLight(const AmbientLight& ambientLight);
    AmbientLight getAmbientLight() const;

    std::shared_ptr<PointLight> makePointLight();
    const std::list<std::shared_ptr<PointLight>>& getPointLights() const;

    private:
    Pool<Drawable2dVariant, 64> mDrawables2d;
    std::list<Drawable3d> mDrawables3d;
    AmbientLight mAmbientLight;
    std::list<std::shared_ptr<PointLight>> mPointLights;
  };
} // namespace Luna
