#include <libluna/Material.hpp>

using namespace Luna;

class Material::impl {
  public:
  ImageResPtr mDiffuse;
  ImageResPtr mNormal;
};

Material::Material() : mImpl{std::make_unique<impl>()} {}

Material::Material(const Material &other) : Material() {
  *mImpl = *other.mImpl;
}

Material &Material::operator=(const Material &other) {
  *mImpl = *other.mImpl;

  return *this;
}

Material::~Material() = default;

void Material::setDiffuse(ImageResPtr image) { mImpl->mDiffuse = image; }

ImageResPtr Material::getDiffuse() const { return mImpl->mDiffuse; }

void Material::setNormal(ImageResPtr image) { mImpl->mNormal = image; }

ImageResPtr Material::getNormal() const { return mImpl->mNormal; }
