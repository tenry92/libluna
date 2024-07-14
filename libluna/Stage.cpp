#include <libluna/Stage.hpp>

using namespace Luna;

class Stage::impl {
  public:
  std::list<SpritePtr> mSprites;
  std::list<TextPtr> mTexts;
  std::list<std::shared_ptr<Model>> mModels;
  AmbientLight mAmbientLight;
  std::list<std::shared_ptr<PointLight>> mPointLights;
};

Stage::Stage() : mImpl{std::make_unique<impl>()} {}

Stage::~Stage() = default;

SpritePtr Stage::makeSprite() {
  auto sprite = Sprite::make();
  mImpl->mSprites.emplace_back(sprite);

  return sprite;
}

const std::list<SpritePtr> &Stage::getSprites() const {
  return mImpl->mSprites;
}

TextPtr Stage::makeText() {
  auto text = Text::make();
  mImpl->mTexts.emplace_back(text);

  return text;
}

const std::list<TextPtr> &Stage::getTexts() const {
  return mImpl->mTexts;
}

std::shared_ptr<Model> Stage::makeModel() {
  auto model = std::make_shared<Model>();
  mImpl->mModels.emplace_back(model);

  return model;
}

const std::list<std::shared_ptr<Model>> &Stage::getModels() const {
  return mImpl->mModels;
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
