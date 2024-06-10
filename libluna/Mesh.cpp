#include <libluna/Mesh.hpp>

using namespace Luna;

class Mesh::impl {
  public:
  std::vector<Vector3f> mVertices;
  std::vector<Face> mFaces;
  std::vector<Vector2f> mTexCoords;
  std::vector<Vector3f> mNormals;
  std::vector<Vector3f> mTangents;
  std::vector<Vector3f> mBiTangents;
};

Mesh::Mesh() : mImpl{std::make_unique<impl>()} {}

Mesh::~Mesh() = default;

std::vector<Vector3f> &Mesh::getVertices() const { return mImpl->mVertices; }

std::vector<Mesh::Face> &Mesh::getFaces() const { return mImpl->mFaces; }

std::vector<Vector2f> &Mesh::getTexCoords() const { return mImpl->mTexCoords; }

std::vector<Vector3f> &Mesh::getNormals() const { return mImpl->mNormals; }

std::vector<Vector3f> &Mesh::getTangents() const { return mImpl->mTangents; }

std::vector<Vector3f> &Mesh::getBitangents() const {
  return mImpl->mBiTangents;
}
