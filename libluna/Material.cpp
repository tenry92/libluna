#include <libluna/Material.hpp>

using namespace Luna;

Material::Material() = default;

Material::~Material() = default;

void Material::setDiffuse(Image *image) { mDiffuse = image; }

Image *Material::getDiffuse() const { return mDiffuse; }

void Material::setNormal(Image *image) { mNormal = image; }

Image *Material::getNormal() const { return mNormal; }
