#pragma once

#include <functional>
#include <map>
#include <memory>

#include <libluna/Image.hpp>
#include <libluna/Stage.hpp>

namespace Luna {
  template <typename TextureType> class TextureCache {
    public:
    using ResourcePtr = std::shared_ptr<ResourceRef<Image>>;
    using ImagePtr = std::shared_ptr<Image>;
    using CallbackType = std::function<TextureType(ImagePtr, int)>;

    TextureCache(std::shared_ptr<Stage> stage, CallbackType loader)
        : mStage{stage}, mLoader{loader} {}

    std::shared_ptr<Stage> getStage() const { return mStage; }

    void updateCache() {
      decltype(mTextures) newTextures;

      for (auto &&sprite : mStage->getSprites()) {
        loadTexture(newTextures, sprite->getImage());
      }

      for (auto &&model : mStage->getModels()) {
        loadTexture(newTextures, model->getMaterial().getDiffuse());
        loadTexture(newTextures, model->getMaterial().getNormal());
      }

      // this will automatically free any previous textures that are not in use
      // anymore
      mTextures = std::move(newTextures);
    }

    inline bool isTextureLoaded(std::shared_ptr<ResourceRef<Image>> resource) {
      return mTextures.count(resource);
    }

    bool hasTextureByImage(std::shared_ptr<ResourceRef<Image>> image) {
      if (!mTextures.count(image)) {
        return false;
      }

      return true;
    }

    TextureType getTextureByImage(
        std::shared_ptr<ResourceRef<Image>> image, int frame = 0
    ) {
      return mTextures.at(image).at(frame);
    }

    bool hasTextureBySprite(std::shared_ptr<Sprite> sprite) {
      if (!sprite->getImage()) {
        return false;
      }

      return hasTextureByImage(sprite->getImage());
    }

    TextureType getTextureBySprite(std::shared_ptr<Sprite> sprite) {
      return getTextureByImage(sprite->getImage(), sprite->getFrame());
    }

    void loadTexture(
        std::map<ResourcePtr, std::vector<TextureType>> &newTextures,
        std::shared_ptr<ResourceRef<Image>> imageRef
    ) {
      if (!imageRef) {
        return;
      }

      if (this->isTextureLoaded(imageRef)) {
        newTextures.emplace(imageRef, mTextures.at(imageRef));
        return;
      }

      auto future = imageRef->get();

      // todo: keep future,
      // check with every update until ref is ready,
      // load image and dispose future
      // if (!imageRef->isReady()) {
      //   continue;
      // }

      auto image = future.get();
      std::vector<TextureType> frames;
      frames.reserve(image->getFrameCount());

      for (int frameIndex = 0; frameIndex < image->getFrameCount();
           ++frameIndex) {
        frames.push_back(mLoader(image, frameIndex));
      }

      newTextures.emplace(imageRef, frames);
    }

    private:
    std::shared_ptr<Stage> mStage;
    CallbackType mLoader;
    std::map<ResourcePtr, std::vector<TextureType>> mTextures;
  };
} // namespace Luna
