#include <cmath>

#include <libluna/Sprite.hpp>

using namespace Luna;

class Sprite::impl {
  public:
  ImageResPtr mImage;
  std::shared_ptr<Resource<Palette>> mPalette;
  Vector2f mPosition;
};

SpritePtr Sprite::make() {
  return SpritePtr(new Sprite());
}

Sprite::Sprite() : mImpl{std::make_unique<impl>()} {}

Sprite::~Sprite() = default;

void Sprite::setImage(ImageResPtr image) {
  mImpl->mImage = image;
}

ImageResPtr Sprite::getImage() const {
  return mImpl->mImage;
}

void Sprite::setPalette(
    ResourcePtr<Palette> palette
) {
  mImpl->mPalette = palette;
}

ResourcePtr<Palette>
Sprite::getPalette() const {
  return mImpl->mPalette;
}

void Sprite::setPosition(const Vector2f &position) {
  mImpl->mPosition = position;
}

Vector2f Sprite::getPosition() const { return mImpl->mPosition; }
