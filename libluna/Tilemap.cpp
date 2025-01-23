#include <libluna/Tilemap.hpp>

using namespace Luna;

Tilemap::Tilemap() = default;

Tilemap::~Tilemap() = default;

void Tilemap::setTileset(Tileset* tileset) { mTileset = tileset; }

Tileset* Tilemap::getTileset() const { return mTileset; }

void Tilemap::resize(Vector2i size) {
  mSize = size;
  mTileData.resize(size.width * size.height);
}

Vector2i Tilemap::getSize() const { return mSize; }

Tilemap::TileData* Tilemap::getTileData() { return mTileData.data(); }
