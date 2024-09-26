#include <libluna/Stage.hpp>

#include <forward_list>

#include <libluna/overloaded.hpp>

using namespace Luna;

class Stage::impl {
  public:
  std::list<Drawable2d> mDrawables2d;
  std::list<Drawable3d> mDrawables3d;
  AmbientLight mAmbientLight;
  std::list<std::shared_ptr<PointLight>> mPointLights;
  TextureCache mTextureCache;
};

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
}

Stage::Stage() : mImpl{std::make_unique<impl>()} {}

Stage::~Stage() = default;

void Stage::add(SpritePtr sprite) { mImpl->mDrawables2d.emplace_back(sprite); }

void Stage::add(TextPtr text) { mImpl->mDrawables2d.emplace_back(text); }

void Stage::add(ModelPtr model) { mImpl->mDrawables3d.emplace_back(model); }

void Stage::remove(SpritePtr sprite) {
  mImpl->mDrawables2d.remove_if([sprite](const auto &drawable) {
    return std::holds_alternative<SpritePtr>(drawable) &&
           std::get<SpritePtr>(drawable) == sprite;
  });
}

void Stage::remove(TextPtr text) {
  mImpl->mDrawables2d.remove_if([text](const auto &drawable) {
    return std::holds_alternative<TextPtr>(drawable) &&
           std::get<TextPtr>(drawable) == text;
  });
}

void Stage::remove(ModelPtr model) { mImpl->mDrawables3d.remove(model); }

const std::list<Stage::Drawable2d> &Stage::getDrawables2d() const {
  return mImpl->mDrawables2d;
}

const std::list<Stage::Drawable3d> &Stage::getDrawables3d() const {
  return mImpl->mDrawables3d;
}

void Stage::setAmbientLight(const AmbientLight &ambientLight) {
  mImpl->mAmbientLight = ambientLight;
}

AmbientLight Stage::getAmbientLight() const { return mImpl->mAmbientLight; }

std::shared_ptr<PointLight> Stage::makePointLight() {
  auto light = std::make_shared<PointLight>();
  mImpl->mPointLights.emplace_back(light);

  return light;
}

const std::list<std::shared_ptr<PointLight>> &Stage::getPointLights() const {
  return mImpl->mPointLights;
}

TextureCache *Stage::getTextureCache() const {
  return &mImpl->mTextureCache;
}

void Stage::updateTextureCache() {
  auto images = listImagesInUse(this);

  mImpl->mTextureCache.resetPriorities();

  for (auto &&imageResPtr : images) {
    mImpl->mTextureCache.addImage(imageResPtr, TextureCache::kForce);
  }
}
