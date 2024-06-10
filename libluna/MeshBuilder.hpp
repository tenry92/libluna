#pragma once

#include <array>

#include <libluna/Mesh.hpp>

namespace Luna {
  class MeshBuilder {
    public:
    /**
     * The vertices are in the order top-left, top-right, bottom-left,
     * bottom-right.
     */
    static void
    addQuadFace(std::shared_ptr<Mesh> mesh, std::array<Vector3f, 4> vertices);
  };
} // namespace Luna
