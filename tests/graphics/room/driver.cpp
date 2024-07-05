#include <vector>
#include <cmath>
#include <cstring>

#include <libgfx/libgfx.h>

#include <libluna/Application.hpp>
#include <libluna/MeshBuilder.hpp>
#include <libluna/InputManager.hpp>
#include <libluna/ResourceReader.hpp>

#ifdef __SWITCH__
#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#else
#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600
#endif

using namespace std;
using namespace Luna;

static int readFromResource(void *dest, size_t size, void *userData) {
  auto *reader = reinterpret_cast<ResourceReader *>(userData);

  return reader->read(reinterpret_cast<uint8_t *>(dest), 1, size);
}

class DummyImageLoader {
  public:
  DummyImageLoader(String filename) : mFilename(filename) {}

  ImagePtr operator()() {
    auto reader = ResourceReader::make(mFilename.c_str());
    auto gfx = libgfx_loadImageFromCallback(readFromResource, reader.get());
    auto image = Image::makeRgb32({gfx->width, gfx->height});
    memcpy(image->getData(), libgfx_getFramePointer(gfx, 0), image->getByteCount());
    libgfx_freeImage(gfx);

    return image;
  }

  private:
  String mFilename;
};

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Stage> stage;
  shared_ptr<Canvas> canvas;

  shared_ptr<Model> model;
  auto mesh = std::make_shared<Mesh>();

  Camera3d camera;
  shared_ptr<PointLight> pointLight;
  float t = 0.f;

  InputManager inputManager;
  inputManager.addButtonBinding("up", "Keyboard/Scancode/E");
  inputManager.addButtonBinding("down", "Keyboard/Scancode/Q");
  inputManager.addButtonBinding("forward", "Keyboard/Scancode/W");
  inputManager.addButtonBinding("backward", "Keyboard/Scancode/S");
  inputManager.addButtonBinding("left", "Keyboard/Scancode/A");
  inputManager.addButtonBinding("right", "Keyboard/Scancode/D");
  inputManager.addButtonBinding("turnLeft", "Keyboard/Scancode/ArrowLeft");
  inputManager.addButtonBinding("turnRight", "Keyboard/Scancode/ArrowRight");
  inputManager.addButtonBinding("fast", "Keyboard/Scancode/LeftShift");

  // floor
  MeshBuilder::addQuadFace(mesh, {{
    {-10.0f, 0.0f, -10.0f}, {10.0f, 0.0f, -10.0f},
    {-10.0f, 0.0f, 10.0f}, {10.0f, 0.0f, 10.0f},
  }});

  // front wall
  MeshBuilder::addQuadFace(mesh, {{
    {-10.0f, 3.0f, -10.0f}, {10.0f, 3.0f, -10.0f},
    {-10.0f, 0.0f, -10.0f}, {10.0f, 0.0f, -10.0f},
  }});

  // left wall
  MeshBuilder::addQuadFace(mesh, {{
    {-10.0f, 3.0f, 10.0f}, {-10.0f, 3.0f, -10.0f},
    {-10.0f, 0.0f, 10.0f}, {-10.0f, 0.0f, -10.0f},
  }});

  // left wall
  MeshBuilder::addQuadFace(mesh, {{
    {10.0f, 3.0f, -10.0f}, {10.0f, 3.0f, 10.0f},
    {10.0f, 0.0f, -10.0f}, {10.0f, 0.0f, 10.0f},
  }});

  // back wall
  MeshBuilder::addQuadFace(mesh, {{
    {10.0f, 3.0f, 10.0f}, {-10.0f, 3.0f, 10.0f},
    {10.0f, 0.0f, 10.0f}, {-10.0f, 0.0f, 10.0f},
  }});

  app.whenReady([&]() {
    stage = make_shared<Stage>();
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    canvas->setStage(stage);

    AmbientLight ambientLight;
    ambientLight.color = {1.0f, 1.0f, 0.8f};
    ambientLight.intensity = 0.3f;
    stage->setAmbientLight(ambientLight);

    canvas->setCamera3d(camera);

    pointLight = stage->makePointLight();

    model = stage->makeModel();
    model->setMesh(mesh);

    auto diffuseRef = make_shared<Resource<Image>>(DummyImageLoader("textures/BricksDragfacedRunning008_COL_1K.gfx"));
    model->getMaterial().setDiffuse(diffuseRef);

    auto normalRef = make_shared<Resource<Image>>(DummyImageLoader("textures/BricksDragfacedRunning008_NRM_1K.gfx"));
    model->getMaterial().setNormal(normalRef);
  });

  camera.setPosition({0.0f, 1.0f, 0.0f});

  app.addInterval(60, [&](float elapsedTime) {
    if (elapsedTime > 1.0f) {
      elapsedTime = 1.0f;
    }

    t += elapsedTime;

    inputManager.update(&canvas->getButtonEvents(), elapsedTime);

    float speed = 1.0f;

    if (inputManager.isButtonHeld("fast")) {
      speed = 4.0f;
    }

    if (inputManager.isButtonHeld("up")) {
      camera.setPosition(camera.getPosition() + Vector3f::up() * elapsedTime * speed);
    }

    if (inputManager.isButtonHeld("down")) {
      camera.setPosition(camera.getPosition() + Vector3f::down() * elapsedTime * speed);
    }

    if (inputManager.isButtonHeld("forward")) {
      camera.setPosition(camera.getPosition() + Vector3f::forward() * elapsedTime * speed);
    }

    if (inputManager.isButtonHeld("backward")) {
      camera.setPosition(camera.getPosition() + Vector3f::backward() * elapsedTime * speed);
    }

    if (inputManager.isButtonHeld("left")) {
      camera.setPosition(camera.getPosition() + Vector3f::left() * elapsedTime * speed);
    }

    if (inputManager.isButtonHeld("right")) {
      camera.setPosition(camera.getPosition() + Vector3f::right() * elapsedTime * speed);
    }

    if (inputManager.isButtonHeld("turnLeft")) {
      camera.rotateY(-elapsedTime * speed);
    }

    if (inputManager.isButtonHeld("turnRight")) {
      camera.rotateY(elapsedTime * speed);
    }

    pointLight->position = {
      sin(t) * 5.0f, 1.0f, cos(t) * 5.0f
    };

    canvas->setCamera3d(camera);
  });

  return app.run();
}
