#pragma once

#include <array>
#include <memory>
#include <vector>

#include <libluna/Vector.hpp>

namespace Luna {
  class Mesh final {
    public:
    using Face = std::array<std::size_t, 3>;

    Mesh();
    ~Mesh();

    std::vector<Vector3f> &getVertices() const;
    std::vector<Face> &getFaces() const;
    std::vector<Vector2f> &getTexCoords() const;
    std::vector<Vector3f> &getNormals() const;
    std::vector<Vector3f> &getTangents() const;
    std::vector<Vector3f> &getBitangents() const;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
