#include <cstring>

#include <libgfx/libgfx.h>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>
#include <libluna/MeshBuilder.hpp>
#include <libluna/InputManager.hpp>
#include <libluna/ResourceReader.hpp>

#ifdef __SWITCH__
#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#elif defined(N64)
#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 240
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

class GfxImageLoader {
  public:
  GfxImageLoader(const String &assetName) : mAssetName(assetName) {}

  ImagePtr operator()() {
    auto reader = ResourceReader::make(mAssetName.c_str());
    auto gfx = libgfx_loadImageFromCallback(readFromResource, reader.get());

    auto frameset = &gfx->framesets[0];

    auto image = Image::makeRgb32({frameset->width, frameset->height});
    memcpy(image->getData(), libgfx_getFramePointer(frameset, 0), image->getByteCount());

    return image;
  }

  private:
  String mAssetName;
};

static void setupInput(InputManager &inputManager) {
  inputManager.addButtonBinding("up", "Keyboard/Scancode/E");
  inputManager.addButtonBinding("up", "Gamepad/N64/CUp");
  inputManager.addButtonBinding("down", "Keyboard/Scancode/Q");
  inputManager.addButtonBinding("down", "Gamepad/N64/CDown");
  inputManager.addButtonBinding("forward", "Keyboard/Scancode/W");
  inputManager.addButtonBinding("forward", "Gamepad/N64/DPadUp");
  inputManager.addButtonBinding("backward", "Keyboard/Scancode/S");
  inputManager.addButtonBinding("backward", "Gamepad/N64/DPadDown");
  inputManager.addButtonBinding("left", "Keyboard/Scancode/A");
  inputManager.addButtonBinding("left", "Gamepad/N64/DPadLeft");
  inputManager.addButtonBinding("right", "Keyboard/Scancode/D");
  inputManager.addButtonBinding("right", "Gamepad/N64/DPadRight");
  inputManager.addButtonBinding("turnLeft", "Keyboard/Scancode/ArrowLeft");
  inputManager.addButtonBinding("turnLeft", "Gamepad/N64/CLeft");
  inputManager.addButtonBinding("turnRight", "Keyboard/Scancode/ArrowRight");
  inputManager.addButtonBinding("turnRight", "Gamepad/N64/CRight");
  inputManager.addButtonBinding("fast", "Keyboard/Scancode/LeftShift");
  inputManager.addButtonBinding("fast", "Gamepad/N64/Z");
}

static MeshPtr makeMesh() {
  auto mesh = make_shared<Mesh>();

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

  return mesh;
}

int main(int argc, char **argv) {
  Application app(argc, argv);

  InputManager inputManager;
  setupInput(inputManager);

  shared_ptr<Canvas> canvas;
  Camera3d camera;
  camera.setPosition({0.0f, 1.0f, 0.0f});
  shared_ptr<PointLight> pointLight;
  float t = 0.f;

  app.whenReady([&]() {
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    canvas->setBackgroundColor({0.2f, 0.1f, 0.0f, 1.0f});

    auto stage = make_shared<Stage>();
    canvas->setStage(stage);

    AmbientLight ambient;
    ambient.color = {1.0f, 1.0f, 0.8f};
    ambient.intensity = 0.3f;
    stage->setAmbientLight(ambient);

    canvas->setCamera3d(camera);

    pointLight = stage->makePointLight();

    auto model = make_shared<Model>();
    model->setMesh(makeMesh());
    stage->add(model);

    auto diffuse = make_shared<Resource<Image>>(GfxImageLoader("wall_32x32.gfx"));
    model->getMaterial().setDiffuse(diffuse);
  });

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
