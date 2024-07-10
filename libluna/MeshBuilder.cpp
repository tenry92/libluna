#include <libluna/MeshBuilder.hpp>

using namespace Luna;

void MeshBuilder::addQuadFace(
    std::shared_ptr<Mesh> mesh, std::array<Vector3f, 4> vertices
) {
  auto indexOffset = mesh->getVertices().size();

  mesh->getVertices().reserve(mesh->getVertices().size() + 4);
  mesh->getVertices().push_back(vertices.at(0)); // top-left
  mesh->getVertices().push_back(vertices.at(1)); // top-right
  mesh->getVertices().push_back(vertices.at(3)); // bottom-right
  mesh->getVertices().push_back(vertices.at(2)); // bottom-left

  mesh->getFaces().push_back({indexOffset + 0, indexOffset + 2, indexOffset + 1}
  );
  mesh->getFaces().push_back({indexOffset + 0, indexOffset + 3, indexOffset + 2}
  );

  mesh->getTexCoords().push_back({0.0f, 0.0f});
  mesh->getTexCoords().push_back({1.0f, 0.0f});
  mesh->getTexCoords().push_back({1.0f, 1.0f});
  mesh->getTexCoords().push_back({0.0f, 1.0f});

  auto edge1 = vertices[1] - vertices[0];
  auto edge2 = vertices[2] - vertices[0];

  auto normal = Vector3f::cross(edge1, vertices[0] - vertices[2]).normalized();
  mesh->getNormals().push_back(normal);
  mesh->getNormals().push_back(normal);
  mesh->getNormals().push_back(normal);

  normal = Vector3f::cross(edge2, vertices[3] - vertices[0]).normalized();
  mesh->getNormals().push_back(normal);

  auto deltaUv1 = mesh->getTexCoords().at(indexOffset + 1) -
                  mesh->getTexCoords().at(indexOffset);
  auto deltaUv2 = mesh->getTexCoords().at(indexOffset + 2) -
                  mesh->getTexCoords().at(indexOffset);

#ifdef N64
  // avoid division by 0 exception
  float f = 1.0f;
#else
  float f = 1.0f / (deltaUv1.x() * deltaUv2.y() - deltaUv2.x() - deltaUv1.y());
#endif

  for (int i = 0; i < 4; ++i) {
    mesh->getTangents().push_back(
        {f * (deltaUv2.y() * edge1.x() - deltaUv1.y() * edge2.x()),
         f * (deltaUv2.y() * edge1.y() - deltaUv1.y() * edge2.y()),
         f * (deltaUv2.y() * edge1.z() - deltaUv1.y() * edge2.z())}
    );

    mesh->getBitangents().push_back(
        {f * (-deltaUv2.x() * edge1.x() + deltaUv1.x() * edge2.x()),
         f * (-deltaUv2.x() * edge1.y() + deltaUv1.x() * edge2.y()),
         f * (-deltaUv2.x() * edge1.z() + deltaUv1.x() * edge2.z())}
    );
  }
}
