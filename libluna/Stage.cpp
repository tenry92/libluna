#include <libluna/Stage.hpp>

using namespace Luna;

class Stage::impl {
  public:
  std::list<Drawable2d> mDrawables2d;
  std::list<Drawable3d> mDrawables3d;
  AmbientLight mAmbientLight;
  std::list<std::shared_ptr<PointLight>> mPointLights;
};

Stage::Stage() : mImpl{std::make_unique<impl>()} {}

Stage::~Stage() = default;

void Stage::add(SpritePtr sprite) {
  mImpl->mDrawables2d.emplace_back(sprite);
}

void Stage::add(TextPtr text) {
  mImpl->mDrawables2d.emplace_back(text);
}

void Stage::add(ModelPtr model) {
  mImpl->mDrawables3d.emplace_back(model);
}

void Stage::remove(SpritePtr sprite) {
  mImpl->mDrawables2d.remove_if([sprite](const auto &drawable) {
    return std::holds_alternative<SpritePtr>(drawable) && std::get<SpritePtr>(drawable) == sprite;
  });
}

void Stage::remove(TextPtr text) {
  mImpl->mDrawables2d.remove_if([text](const auto &drawable) {
    return std::holds_alternative<TextPtr>(drawable) && std::get<TextPtr>(drawable) == text;
  });
}

void Stage::remove(ModelPtr model) {
  mImpl->mDrawables3d.remove(model);
}

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
