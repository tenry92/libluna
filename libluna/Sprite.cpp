#include <cmath>

#include <libluna/Sprite.hpp>

using namespace Luna;

Sprite::Sprite() = default;

Sprite::~Sprite() = default;

void Sprite::setImageLoader(ImageLoader* loader) { mImageLoader = loader; }

ImageLoader* Sprite::getImageLoader() const { return mImageLoader; }

void Sprite::setPalette(ResourcePtr<Palette> palette) { mPalette = palette; }

ResourcePtr<Palette> Sprite::getPalette() const { return mPalette; }
