#pragma once

#include <vector>

#include <libluna/Image.hpp>

namespace Luna {
  class Tileset final {
    public:
    Tileset();
    ~Tileset();

    void setTileSize(int tileSize);
    int getTileSize() const;

    void setImage(Image *image);
    Image *getImage() const;

    inline int getColumns() const {
      return mImage->getSize().width / mTileSize;
    }

    private:
    int mTileSize;
    Image *mImage{nullptr};
  };
} // namespace Luna
