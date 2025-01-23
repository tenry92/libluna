#include <libluna/Tileset.hpp>

using namespace Luna;

Tileset::Tileset() = default;

Tileset::~Tileset() = default;

void Tileset::setTileSize(int tileSize) { mTileSize = tileSize; }

int Tileset::getTileSize() const { return mTileSize; }

void Tileset::setImage(ImageLoader* image) { mImage = image; }

ImageLoader* Tileset::getImage() const { return mImage; }
