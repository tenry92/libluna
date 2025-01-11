#include <cmath>

#include <libluna/Sprite.hpp>

using namespace Luna;

Sprite::Sprite() = default;

Sprite::~Sprite() = default;

void Sprite::setImageLoader(ImageLoader *loader) { mImageLoader = loader; }

ImageLoader *Sprite::getImageLoader() const { return mImageLoader; }

void Sprite::setPalette(ResourcePtr<Palette> palette) { mPalette = palette; }

ResourcePtr<Palette> Sprite::getPalette() const { return mPalette; }

void Sprite::setPosition(const Vector2f &position) { mPosition = position; }

Vector2f Sprite::getPosition() const { return mPosition; }

void Sprite::setPriority(float priority) { mPriority = priority; }

float Sprite::getPriority() const { return mPriority; }

void Sprite::setVisible(bool visible) { mVisible = visible; }

bool Sprite::isVisible() const { return mVisible; }
