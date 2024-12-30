#pragma once

#include <vector>

#include <libluna/ImageLoader.hpp>
#include <libluna/Sprite.hpp>

namespace Luna {
  class Tileset final {
    public:
    Tileset();
    ~Tileset();

    void setTileSize(int tileSize);
    int getTileSize() const;

    void setImage(ImageLoader *image);
    ImageLoader *getImage() const;

    inline int getColumns() const {
      // return mImage->getSize().width / mTileSize;
      return 1;
    }

    private:
    int mTileSize;
    ImageLoader *mImage{nullptr};
  };
} // namespace Luna
