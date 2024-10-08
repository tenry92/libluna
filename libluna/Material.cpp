#include <libluna/Material.hpp>

using namespace Luna;

Material::Material() = default;

Material::~Material() = default;

void Material::setDiffuse(ImageResPtr image) { mDiffuse = image; }

ImageResPtr Material::getDiffuse() const { return mDiffuse; }

void Material::setNormal(ImageResPtr image) { mNormal = image; }

ImageResPtr Material::getNormal() const { return mNormal; }
