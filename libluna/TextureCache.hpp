#pragma once

#include <unordered_map>

#include <libluna/Image.hpp>

namespace Luna {
  class TextureCache {
    public:
    enum Priority {
      /**
       * @brief Force texture to be loaded.
       *
       * This is used internally for drawables, that are currently visible.
       */
      kForce,

      /**
       * @brief Texture should be preloaded if possible.
       *
       * This should be used for textures that might not be visible right now,
       * such as other animation frames of a sprite.
       */
      kHigh,

      kLow,
    };

    TextureCache();
    ~TextureCache();

    void addImage(ImageResPtr image, Priority priority = kHigh);
    void removeImage(ImageResPtr image);
    void resetPriorities();
    const std::unordered_map<ImageResPtr, Priority> &getCache() const;

    private:
    std::unordered_map<ImageResPtr, Priority> mCache;
  };
} // namespace Luna
