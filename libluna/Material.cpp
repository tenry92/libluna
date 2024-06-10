#include <libluna/Material.hpp>

using namespace Luna;

class Material::impl {
  public:
  std::shared_ptr<ResourceRef<Image>> mDiffuse;
  std::shared_ptr<ResourceRef<Image>> mNormal;
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

void Material::setDiffuse(std::shared_ptr<ResourceRef<Image>> image) {
  mImpl->mDiffuse = image;
}

std::shared_ptr<ResourceRef<Image>> Material::getDiffuse() const {
  return mImpl->mDiffuse;
}

void Material::setNormal(std::shared_ptr<ResourceRef<Image>> image) {
  mImpl->mNormal = image;
}

std::shared_ptr<ResourceRef<Image>> Material::getNormal() const {
  return mImpl->mNormal;
}
