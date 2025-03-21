#include <libluna/TextureCache.hpp>

using namespace Luna;

TextureCache::TextureCache() = default;

TextureCache::~TextureCache() = default;

void TextureCache::addImage(ImageLoader* image, Priority priority) {
  mCache.insert_or_assign(image, priority);
}

void TextureCache::removeImage(ImageLoader* image) { mCache.erase(image); }

void TextureCache::resetPriorities() {
  for (const auto& [image, priority] : mCache) {
    if (priority == kForce) {
      mCache.insert_or_assign(image, kHigh);
    }
  }
}

const std::unordered_map<ImageLoader*, TextureCache::Priority>&
TextureCache::getCache() const {
  return mCache;
}
