#include <libluna/Material.hpp>

using namespace Luna;

Material::Material() = default;

Material::~Material() = default;

void Material::setDiffuse(ImageLoader* image) { mDiffuse = image; }

ImageLoader* Material::getDiffuse() const { return mDiffuse; }

void Material::setNormal(ImageLoader* image) { mNormal = image; }

ImageLoader* Material::getNormal() const { return mNormal; }
