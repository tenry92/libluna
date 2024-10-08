#include <libluna/Stage.hpp>

#include <forward_list>

#include <libluna/overloaded.hpp>

using namespace Luna;

namespace {
  std::forward_list<ImageResPtr> listImagesInUse(Stage *stage) {
    std::forward_list<ImageResPtr> images;

    for (auto &&drawable : stage->getDrawables2d()) {
      std::visit(
          overloaded{
              [](auto) {},
              [&](SpritePtr sprite) {
                if (sprite->getImage()) {
                  images.emplace_front(sprite->getImage());
                }
              }},
          drawable
      );
    }

    // todo: font characters

    for (auto &&model : stage->getDrawables3d()) {
      auto diffuse = model->getMaterial().getDiffuse();
      auto normal = model->getMaterial().getNormal();

      if (diffuse) {
        images.emplace_front(diffuse);
      }

      if (normal) {
        images.emplace_front(normal);
      }
    }

    return images;
  }
} // namespace

Stage::Stage() = default;

Stage::~Stage() = default;

void Stage::add(SpritePtr sprite) { mDrawables2d.emplace_back(sprite); }

void Stage::add(TextPtr text) { mDrawables2d.emplace_back(text); }

void Stage::add(ModelPtr model) { mDrawables3d.emplace_back(model); }

void Stage::remove(SpritePtr sprite) {
  mDrawables2d.remove_if([sprite](const auto &drawable) {
    return std::holds_alternative<SpritePtr>(drawable) &&
           std::get<SpritePtr>(drawable) == sprite;
  });
}

void Stage::remove(TextPtr text) {
  mDrawables2d.remove_if([text](const auto &drawable) {
    return std::holds_alternative<TextPtr>(drawable) &&
           std::get<TextPtr>(drawable) == text;
  });
}

void Stage::remove(ModelPtr model) { mDrawables3d.remove(model); }

const std::list<Stage::Drawable2d> &Stage::getDrawables2d() const {
  return mDrawables2d;
}

const std::list<Stage::Drawable3d> &Stage::getDrawables3d() const {
  return mDrawables3d;
}

void Stage::setAmbientLight(const AmbientLight &ambientLight) {
  mAmbientLight = ambientLight;
}

AmbientLight Stage::getAmbientLight() const { return mAmbientLight; }

std::shared_ptr<PointLight> Stage::makePointLight() {
  auto light = std::make_shared<PointLight>();
  mPointLights.emplace_back(light);

  return light;
}

const std::list<std::shared_ptr<PointLight>> &Stage::getPointLights() const {
  return mPointLights;
}

TextureCache *Stage::getTextureCache() { return &mTextureCache; }

void Stage::updateTextureCache() {
  auto images = listImagesInUse(this);

  mTextureCache.resetPriorities();

  for (auto &&imageResPtr : images) {
    mTextureCache.addImage(imageResPtr, TextureCache::kForce);
  }
}
