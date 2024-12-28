#pragma once

#include <cstdint>
#include <vector>

#include <libluna/Tileset.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Tilemap final {
    public:
    using TileData = std::uint16_t;

    Tilemap();
    ~Tilemap();

    void setTileset(Tileset *tileset);
    Tileset *getTileset() const;

    void resize(Vector2i size);
    Vector2i getSize() const;

    TileData *getTileData();

    inline TileData &at(Vector2i position) {
      return mTileData[position.y * mSize.width + position.x];
    }

    inline TileData at(Vector2i position) const {
      return mTileData[position.y * mSize.width + position.x];
    }

    private:
    Tileset *mTileset;
    Vector2i mSize;
    std::vector<TileData> mTileData;
  };
} // namespace Luna
