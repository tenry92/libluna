#include <libluna/Stage.hpp>

#include <forward_list>

#include <libluna/overloaded.hpp>

using namespace Luna;

namespace {
  std::forward_list<ImageLoader *> listImagesInUse(Stage *stage) {
    std::forward_list<ImageLoader *> imageLoaders;

    for (auto &&drawable : stage->getDrawables2d()) {
      std::visit(
          overloaded{
              [](auto) {},
              [&](const Sprite &sprite) {
                if (sprite.getImageLoader()) {
                  imageLoaders.emplace_front(sprite.getImageLoader());
                }
              },
              [&](const Tilemap &tilemap) {
                auto tileset = tilemap.getTileset();
                if (tileset->getImage()) {
                  imageLoaders.emplace_front(tileset->getImage());
                }
              },
          },
          drawable
      );
    }

    // todo: font characters

    for (auto &&model : stage->getDrawables3d()) {
      auto diffuse = model->getMaterial().getDiffuse();
      auto normal = model->getMaterial().getNormal();

      if (diffuse) {
        imageLoaders.emplace_front(diffuse);
      }

      if (normal) {
        imageLoaders.emplace_front(normal);
      }
    }

    return imageLoaders;
  }
} // namespace

Stage::Stage() = default;

Stage::~Stage() = default;

Sprite *Stage::createSprite() {
  auto drawable2d = mDrawables2d.acquire();
  drawable2d->emplace<Sprite>();

  return &std::get<Sprite>(*drawable2d);
}

void Stage::destroySprite(Sprite *sprite) {
  for (auto &&drawable : mDrawables2d) {
    if (std::holds_alternative<Sprite>(drawable) &&
        &std::get<Sprite>(drawable) == sprite) {
      mDrawables2d.release(&drawable);
      break;
    }
  }
}

Text *Stage::createText() {
  auto drawable2d = mDrawables2d.acquire();
  drawable2d->emplace<Text>();

  return &std::get<Text>(*drawable2d);
}

void Stage::destroyText(Text *text) {
  for (auto &&drawable : mDrawables2d) {
    if (std::holds_alternative<Text>(drawable) &&
        &std::get<Text>(drawable) == text) {
      mDrawables2d.release(&drawable);
      break;
    }
  }
}

Tilemap *Stage::createTilemap() {
  auto drawable2d = mDrawables2d.acquire();
  drawable2d->emplace<Tilemap>();

  return &std::get<Tilemap>(*drawable2d);
}

void Stage::destroyTilemap(Tilemap *tilemap) {
  for (auto &&drawable : mDrawables2d) {
    if (std::holds_alternative<Tilemap>(drawable) &&
        &std::get<Tilemap>(drawable) == tilemap) {
      mDrawables2d.release(&drawable);
      break;
    }
  }
}

Model *Stage::createModel() {
  // todo: use smart pointers
  auto model = new Model();
  mDrawables3d.emplace_back(model);

  return model;
}

void Stage::destroyModel(Model *model) {
  mDrawables3d.remove(model);
  delete model;
}

const Pool<Stage::Drawable2d, 32> &Stage::getDrawables2d() const {
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
