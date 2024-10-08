#include <libluna/Model.hpp>

using namespace Luna;

Model::Model() : mTransform{Matrix4x4::identity()} {}

Model::~Model() = default;

void Model::setMesh(std::shared_ptr<Mesh> mesh) { mMesh = mesh; }

std::shared_ptr<Mesh> Model::getMesh() const { return mMesh; }

Matrix4x4 &Model::getTransform() { return mTransform; }

void Model::setMaterial(Material material) { mMaterial = material; }

Material &Model::getMaterial() { return mMaterial; }
