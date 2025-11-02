#pragma once

#include <vector>

#include <libluna/Vector.hpp>

namespace Luna {
  class Tileset final {
    public:
    Tileset();
    ~Tileset();

    void setTileSize(int tileSize);
    int getTileSize() const;

    void setTextureId(int textureId);
    int getTextureId() const;

    void setTextureSize(Vector2i size);
    Vector2i getTextureSize() const;

    inline int getColumns() const {
      if (mTileSize == 0) {
        return 0;
      }
      return mTextureSize.x / mTileSize;
    }

    private:
    int mTileSize{0};
    int mTextureId{0};
    Vector2i mTextureSize{0, 0};
  };
} // namespace Luna
