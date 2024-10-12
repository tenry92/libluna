#include <libluna/Stage.hpp>

#include <forward_list>

#include <libluna/overloaded.hpp>

using namespace Luna;

namespace {
  std::forward_list<Image *> listImagesInUse(Stage *stage) {
    std::forward_list<Image *> images;

    for (auto &&drawable : stage->getDrawables2d()) {
      std::visit(
          overloaded{
              [](auto) {},
              [&](Sprite *sprite) {
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

Sprite *Stage::createSprite() {
  auto sprite = new Sprite();
  mDrawables2d.emplace_back(sprite);

  return sprite;
}

void Stage::destroySprite(Sprite *sprite) {
  mDrawables2d.remove_if([sprite](const auto &drawable) {
    return std::holds_alternative<Sprite *>(drawable) &&
           std::get<Sprite *>(drawable) == sprite;
  });
}

Text *Stage::createText() {
  auto text = new Text();
  mDrawables2d.emplace_back(text);

  return text;
}

void Stage::destroyText(Text *text) {
  mDrawables2d.remove_if([text](const auto &drawable) {
    return std::holds_alternative<Text *>(drawable) &&
           std::get<Text *>(drawable) == text;
  });
}

Model *Stage::createModel() {
  auto model = new Model();
  mDrawables3d.emplace_back(model);

  return model;
}

void Stage::destroyModel(Model *model) {
  mDrawables3d.remove(model);
}

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

  for (auto &&image : images) {
    mTextureCache.addImage(image, TextureCache::kForce);
  }
}
