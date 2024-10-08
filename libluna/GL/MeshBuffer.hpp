#pragma once

#include <libluna/GL/common.hpp>
#include <libluna/Mesh.hpp>
#include <libluna/Vector.hpp>

namespace Luna::GL {
  class MeshBuffer {
    public:
    MeshBuffer(std::shared_ptr<Luna::Mesh> mesh) {
      unsigned int buffers[2];
      CHECK_GL(glGenBuffers(2, buffers));
      mVertexBuffer = buffers[0];
      mElementBuffer = buffers[1];
      CHECK_GL(glGenVertexArrays(1, &mVertexAttribConf));

      bind();
      load(mesh);
      configureVertexAttributes();
      unbind();
    }

    ~MeshBuffer() {
      unbind();
      unsigned int buffers[] = {mVertexBuffer, mElementBuffer};
      CHECK_GL(glDeleteBuffers(2, buffers));
      CHECK_GL(glDeleteVertexArrays(1, &mVertexAttribConf));
    }

    MeshBuffer(const MeshBuffer &other) = delete;

    void load(std::shared_ptr<Mesh> mesh) {
      // vertices: [[x, y, z, u, v, nx, ny, nz], ...]
      std::vector<float> vertices;
      vertices.reserve(mesh->getVertices().size() * 14);

      for (int i = 0; i < static_cast<int>(mesh->getVertices().size()); ++i) {
        auto &&vertex = mesh->getVertices().at(i);
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);

        if (i < static_cast<int>(mesh->getTexCoords().size())) {
          auto &&texCoords = mesh->getTexCoords().at(i);
          vertices.push_back(texCoords.x); // u
          vertices.push_back(texCoords.y); // v
        } else {
          vertices.push_back(0.0f); // u
          vertices.push_back(0.0f); // v
        }

        if (i < static_cast<int>(mesh->getNormals().size())) {
          auto &&normal = mesh->getNormals().at(i);
          vertices.push_back(normal.x);
          vertices.push_back(normal.y);
          vertices.push_back(normal.z);
        } else {
          vertices.push_back(0.0f);
          vertices.push_back(0.0f);
          vertices.push_back(0.0f);
        }

        if (i < static_cast<int>(mesh->getTangents().size())) {
          auto &&tangent = mesh->getTangents().at(i);
          vertices.push_back(tangent.x);
          vertices.push_back(tangent.y);
          vertices.push_back(tangent.z);
        } else {
          vertices.push_back(0.0f);
          vertices.push_back(0.0f);
          vertices.push_back(0.0f);
        }

        if (i < static_cast<int>(mesh->getBitangents().size())) {
          auto &&bitangent = mesh->getBitangents().at(i);
          vertices.push_back(bitangent.x);
          vertices.push_back(bitangent.y);
          vertices.push_back(bitangent.z);
        } else {
          vertices.push_back(0.0f);
          vertices.push_back(0.0f);
          vertices.push_back(0.0f);
        }
      }

      CHECK_GL(glBufferData(
          GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(),
          GL_STATIC_DRAW
      ));

      std::vector<unsigned int> indices;
      indices.reserve(mesh->getFaces().size() * 3);

      for (auto &&face : mesh->getFaces()) {
        indices.push_back(static_cast<unsigned int>(face.at(0)));
        indices.push_back(static_cast<unsigned int>(face.at(1)));
        indices.push_back(static_cast<unsigned int>(face.at(2)));
      }

      CHECK_GL(glBufferData(
          GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
          indices.data(), GL_STATIC_DRAW
      ));

      mIndexCount = static_cast<unsigned int>(indices.size());
    }

    void bind() {
      CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer));
      CHECK_GL(glBindVertexArray(mVertexAttribConf));
      CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBuffer));
    }

    void unbind() {
      CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
      CHECK_GL(glBindVertexArray(0));
      CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    void draw() {
      CHECK_GL(glDrawElements(
          GL_TRIANGLES, static_cast<GLsizei>(mIndexCount), GL_UNSIGNED_INT, 0
      ));
    }

    private:
    void configureVertexAttributes() {
      int stride = 14 * sizeof(float);

      // aPos
      CHECK_GL(glVertexAttribPointer(
          0,        // input location = 0
          3,        // vec3
          GL_FLOAT, // vector of floats
          GL_FALSE, // don't normalize input data
          stride,
          (void *)0 // offset; where data begins in the buffer
      ));
      CHECK_GL(glEnableVertexAttribArray(0));

      // aTexCoord
      CHECK_GL(glVertexAttribPointer(
          1,        // input location = 1
          2,        // vec2
          GL_FLOAT, // vector of floats
          GL_FALSE, // don't normalize input data
          stride,
          (void *)(3 * sizeof(float)) // offset; where data begins in the buffer
      ));
      CHECK_GL(glEnableVertexAttribArray(1));

      // aNormal
      CHECK_GL(glVertexAttribPointer(
          2,        // input location = 2
          3,        // vec3
          GL_FLOAT, // vector of floats
          GL_TRUE,  // normalize input data
          stride,
          (void *)(5 * sizeof(float)) // offset; where data begins in the buffer
      ));
      CHECK_GL(glEnableVertexAttribArray(2));
    }

    unsigned int mIndexCount;
    unsigned int mVertexBuffer;
    unsigned int mElementBuffer;
    unsigned int mVertexAttribConf;
  };
} // namespace Luna::GL
