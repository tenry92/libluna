#pragma once

#include <memory>

#include <libluna/Material.hpp>
#include <libluna/Matrix.hpp>
#include <libluna/Mesh.hpp>

namespace Luna {
  class Model final {
    public:
    Model();
    ~Model();

    void setMesh(std::shared_ptr<Mesh> mesh);
    std::shared_ptr<Mesh> getMesh() const;

    void setMaterial(Material material);
    Material &getMaterial() const;

    Matrix4x4 &getTransform() const;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
