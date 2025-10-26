#include <cmath>

#include <libluna/Sprite.hpp>

using namespace Luna;

Sprite::Sprite() = default;

Sprite::~Sprite() = default;

void Sprite::setTexture(int id) { mTextureId = id; }

int Sprite::getTexture() const { return mTextureId; }
