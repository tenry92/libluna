#include <cmath>

#include <libluna/Sprite.hpp>

using namespace Luna;

Sprite::Sprite() = default;

Sprite::~Sprite() = default;

void Sprite::setImage(Image *image) { mImage = image; }

Image *Sprite::getImage() const { return mImage; }

void Sprite::setPalette(ResourcePtr<Palette> palette) { mPalette = palette; }

ResourcePtr<Palette> Sprite::getPalette() const { return mPalette; }

void Sprite::setPosition(const Vector2f &position) { mPosition = position; }

Vector2f Sprite::getPosition() const { return mPosition; }
