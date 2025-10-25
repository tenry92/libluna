#include <libluna/Stage.hpp>

#include <algorithm>
#include <forward_list>

#include <libluna/overloaded.hpp>

using namespace Luna;

Stage::Stage() = default;

Stage::~Stage() = default;

Sprite* Stage::allocSprite() {
  auto drawable2d = mDrawables2d.acquire();

  if (!drawable2d) {
    return nullptr;
  }

  drawable2d->emplace<Sprite>();

  return &std::get<Sprite>(*drawable2d);
}

void Stage::freeSprite(Sprite* sprite) {
  for (auto&& drawable : mDrawables2d) {
    if (std::holds_alternative<Sprite>(drawable) && &std::get<Sprite>(drawable) == sprite) {
      mDrawables2d.release(&drawable);
      break;
    }
  }
}

Primitive* Stage::allocPrimitive() {
  auto drawable2d = mDrawables2d.acquire();

  if (!drawable2d) {
    return nullptr;
  }

  drawable2d->emplace<Primitive>();

  return &std::get<Primitive>(*drawable2d);
}

void Stage::freePrimitive(Primitive* primitive) {
  for (auto&& drawable : mDrawables2d) {
    if (std::holds_alternative<Primitive>(drawable) && &std::get<Primitive>(drawable) == primitive) {
      mDrawables2d.release(&drawable);
      break;
    }
  }
}

Text* Stage::allocText() {
  auto drawable2d = mDrawables2d.acquire();

  if (!drawable2d) {
    return nullptr;
  }

  drawable2d->emplace<Text>();

  return &std::get<Text>(*drawable2d);
}

void Stage::freeText(Text* text) {
  for (auto&& drawable : mDrawables2d) {
    if (std::holds_alternative<Text>(drawable) && &std::get<Text>(drawable) == text) {
      mDrawables2d.release(&drawable);
      break;
    }
  }
}

Tilemap* Stage::allocTilemap() {
  auto drawable2d = mDrawables2d.acquire();

  if (!drawable2d) {
    return nullptr;
  }

  drawable2d->emplace<Tilemap>();

  return &std::get<Tilemap>(*drawable2d);
}

void Stage::freeTilemap(Tilemap* tilemap) {
  for (auto&& drawable : mDrawables2d) {
    if (std::holds_alternative<Tilemap>(drawable) && &std::get<Tilemap>(drawable) == tilemap) {
      mDrawables2d.release(&drawable);
      break;
    }
  }
}

Model* Stage::allocModel() {
  // todo: use smart pointers
  auto model = new Model();
  mDrawables3d.emplace_back(model);

  return model;
}

void Stage::freeModel(Model* model) {
  mDrawables3d.remove(model);
  delete model;
}

const Pool<Stage::Drawable2dVariant, 64>& Stage::getDrawables2d() const {
  return mDrawables2d;
}

const std::forward_list<const Stage::Drawable2dVariant*>
Stage::getSortedDrawables2d() const {
  std::forward_list<const Drawable2dVariant*> sortedDrawables;

  for (auto&& drawable : mDrawables2d) {
    sortedDrawables.emplace_front(&drawable);
  }

  // forward_list.tcc:498:29: null pointer dereference
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
  sortedDrawables.sort(
    [](const Drawable2dVariant* a, const Drawable2dVariant* b) {
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
      float priorityA = 0;
      float priorityB = 0;

      if (std::holds_alternative<Sprite>(*a)) {
        priorityA = std::get<Sprite>(*a).getPriority();
      }

      if (std::holds_alternative<Primitive>(*a)) {
        priorityA = std::get<Primitive>(*a).getPriority();
      }

      if (std::holds_alternative<Text>(*a)) {
        priorityA = std::get<Text>(*a).getPriority();
      }

      if (std::holds_alternative<Sprite>(*b)) {
        priorityB = std::get<Sprite>(*b).getPriority();
      }

      if (std::holds_alternative<Primitive>(*b)) {
        priorityB = std::get<Primitive>(*b).getPriority();
      }

      if (std::holds_alternative<Text>(*b)) {
        priorityB = std::get<Text>(*b).getPriority();
      }

      return priorityA < priorityB;
    }
  );

  return sortedDrawables;
}

const std::list<Stage::Drawable3d>& Stage::getDrawables3d() const {
  return mDrawables3d;
}

void Stage::setAmbientLight(const AmbientLight& ambientLight) {
  mAmbientLight = ambientLight;
}

AmbientLight Stage::getAmbientLight() const { return mAmbientLight; }

std::shared_ptr<PointLight> Stage::makePointLight() {
  auto light = std::make_shared<PointLight>();
  mPointLights.emplace_back(light);

  return light;
}

const std::list<std::shared_ptr<PointLight>>& Stage::getPointLights() const {
  return mPointLights;
}
