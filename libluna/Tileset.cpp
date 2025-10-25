#include <libluna/Tileset.hpp>

using namespace Luna;

Tileset::Tileset() = default;

Tileset::~Tileset() = default;

void Tileset::setTileSize(int tileSize) { mTileSize = tileSize; }

int Tileset::getTileSize() const { return mTileSize; }

void Tileset::setTextureId(int textureId) { mTextureId = textureId; }

int Tileset::getTextureId() const { return mTextureId; }

void Tileset::setTextureSize(Vector2i size) { mTextureSize = size; }

Vector2i Tileset::getTextureSize() const { return mTextureSize; }

