#include <libluna/Material.hpp>

using namespace Luna;

Material::Material() = default;

Material::~Material() = default;

void Material::setDiffuseTexture(int slot) { mDiffuseTexture = slot; }

int Material::getDiffuseTexture() const { return mDiffuseTexture; }

void Material::setNormalTexture(int slot) { mNormalTexture = slot; }

int Material::getNormalTexture() const { return mNormalTexture; }
