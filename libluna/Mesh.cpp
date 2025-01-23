#include <libluna/Mesh.hpp>

using namespace Luna;

Mesh::Mesh() = default;

Mesh::~Mesh() = default;

std::vector<Vector3f>& Mesh::getVertices() { return mVertices; }

std::vector<Mesh::Face>& Mesh::getFaces() { return mFaces; }

std::vector<Vector2f>& Mesh::getTexCoords() { return mTexCoords; }

std::vector<Vector3f>& Mesh::getNormals() { return mNormals; }

std::vector<Vector3f>& Mesh::getTangents() { return mTangents; }

std::vector<Vector3f>& Mesh::getBitangents() { return mBiTangents; }
