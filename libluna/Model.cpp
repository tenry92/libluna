#include <libluna/Model.hpp>

using namespace Luna;

class Model::impl {
  public:
  std::shared_ptr<Mesh> mMesh;
  Matrix4x4 mTransform;
  Material mMaterial;
};

Model::Model() : mImpl{std::make_unique<impl>()} {
  mImpl->mTransform = Matrix4x4::identity();
}

Model::~Model() = default;

void Model::setMesh(std::shared_ptr<Mesh> mesh) { mImpl->mMesh = mesh; }

std::shared_ptr<Mesh> Model::getMesh() const { return mImpl->mMesh; }

Matrix4x4 &Model::getTransform() const { return mImpl->mTransform; }

void Model::setMaterial(Material material) { mImpl->mMaterial = material; }

Material &Model::getMaterial() const { return mImpl->mMaterial; }
