#pragma once

#include <array>
#include <memory>
#include <vector>

#include <libluna/Vector.hpp>

namespace Luna {
  class Mesh;

  class Mesh final {
    public:
    using Face = std::array<uint32_t, 3>;

    Mesh();
    ~Mesh();

    std::vector<Vector3f>& getVertices();
    std::vector<Face>& getFaces();
    std::vector<Vector2f>& getTexCoords();
    std::vector<Vector3f>& getNormals();
    std::vector<Vector3f>& getTangents();
    std::vector<Vector3f>& getBitangents();

    private:
    std::vector<Vector3f> mVertices;
    std::vector<Face> mFaces;
    std::vector<Vector2f> mTexCoords;
    std::vector<Vector3f> mNormals;
    std::vector<Vector3f> mTangents;
    std::vector<Vector3f> mBiTangents;
  };
} // namespace Luna
