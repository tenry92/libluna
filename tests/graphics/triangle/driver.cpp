#include <vector>
#include <cmath>

#include <libluna/Application.hpp>
#include <libluna/MeshBuilder.hpp>

#ifdef __SWITCH__
#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#else
#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600
#endif

using namespace std;
using namespace Luna;

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Stage> stage;
  shared_ptr<Canvas> canvas;

  shared_ptr<Model> triangleModel;
  auto triangleMesh = std::make_shared<Mesh>();
  shared_ptr<Model> quadModel;
  auto quadMesh = std::make_shared<Mesh>();

  triangleMesh->getVertices().reserve(3);
  triangleMesh->getVertices().push_back({-0.5f, 0.25f, 0.0f});
  triangleMesh->getVertices().push_back({-0.25f, 0.75f, 0.0f});
  triangleMesh->getVertices().push_back({-0.75f, 0.75f, 0.0f});

  triangleMesh->getFaces() = {
    {0, 1, 2}
  };

  MeshBuilder::addQuadFace(quadMesh, {{
    {0.25f, 0.75f, 0.0f}, {0.75f, 0.75f, 0.0f},
    {0.25f, 0.25f, 0.0f}, {0.75f, 0.25f, 0.0f},
  }});

  app.whenReady([&]() {
    stage = make_shared<Stage>();
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    canvas->setStage(stage);

    triangleModel = stage->makeModel();
    triangleModel->setMesh(triangleMesh);

    quadModel = stage->makeModel();
    quadModel->setMesh(quadMesh);
  });

  auto transform = Matrix4x4::identity();

  app.addInterval(60, [&](float elapsedTime) {
    if (elapsedTime > 1.0f) {
      elapsedTime = 1.0f;
    }

    transform = transform.rotateX(0.5f * elapsedTime);
    triangleModel->getTransform() = transform;
    quadModel->getTransform() = transform;
  });

  return app.run();
}
