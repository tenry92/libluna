#include <libluna/TextureCache.hpp>

using namespace Luna;

TextureCache::TextureCache() = default;

TextureCache::~TextureCache() = default;

void TextureCache::addImage(ImageResPtr image, Priority priority) {
  mCache.insert_or_assign(image, priority);
}

void TextureCache::removeImage(ImageResPtr image) {
  mCache.erase(image);
}

void TextureCache::resetPriorities() {
  for (const auto &[image, priority] : mCache) {
    if (priority == kForce) {
      mCache.insert_or_assign(image, kHigh);
    }
  }
}

const std::unordered_map<ImageResPtr, TextureCache::Priority> &TextureCache::getCache() const {
  return mCache;
}
