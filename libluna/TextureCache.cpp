#include <libluna/TextureCache.hpp>

using namespace Luna;

class TextureCache::impl {
  public:
  std::unordered_map<ImageResPtr, Priority> mCache;
};

TextureCache::TextureCache() : mImpl{std::make_unique<impl>()} {}

TextureCache::~TextureCache() = default;

void TextureCache::addImage(ImageResPtr image, Priority priority) {
  mImpl->mCache.insert_or_assign(image, priority);
}

void TextureCache::removeImage(ImageResPtr image) {
  mImpl->mCache.erase(image);
}

void TextureCache::resetPriorities() {
  for (const auto &[image, priority] : mImpl->mCache) {
    if (priority == kForce) {
      mImpl->mCache.insert_or_assign(image, kHigh);
    }
  }
}

std::unordered_map<ImageResPtr, TextureCache::Priority> &TextureCache::getCache() const {
  return mImpl->mCache;
}
