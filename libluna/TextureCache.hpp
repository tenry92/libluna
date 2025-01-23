#pragma once

#include <unordered_map>

#include <libluna/ImageLoader.hpp>
#include <libluna/Sprite.hpp>

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

    void addImage(ImageLoader* image, Priority priority = kHigh);
    void removeImage(ImageLoader* image);
    void resetPriorities();
    const std::unordered_map<ImageLoader*, Priority>& getCache() const;

    private:
    std::unordered_map<ImageLoader*, Priority> mCache;
  };
} // namespace Luna
