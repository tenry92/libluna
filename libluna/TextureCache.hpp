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

    void addImage(Image *image, Priority priority = kHigh);
    void removeImage(Image *image);
    void resetPriorities();
    const std::unordered_map<Image *, Priority> &getCache() const;

    private:
    std::unordered_map<Image *, Priority> mCache;
  };
} // namespace Luna
