#pragma once

#include <libluna/Material.hpp>
#include <libluna/Matrix.hpp>
#include <libluna/Mesh.hpp>

namespace Luna {
  class Model;

  /**
   * @brief A 3D model consisting of a mesh and a material.
   *
   * @ingroup canvas
   */
  class Model final {
    public:
    Model();
    ~Model();

    void setMesh(std::shared_ptr<Mesh> mesh);
    std::shared_ptr<Mesh> getMesh() const;

    void setMaterial(Material material);
    Material& getMaterial();

    Matrix4x4& getTransform();

    private:
    std::shared_ptr<Mesh> mMesh;
    Matrix4x4 mTransform;
    Material mMaterial;
  };
} // namespace Luna
