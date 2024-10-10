#include <cmath>
#include <cstring>

#include <libgfx/libgfx.h>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>
#include <libluna/InputManager.hpp>
#include <libluna/Logger.hpp>
#include <libluna/MeshBuilder.hpp>
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
    memcpy(
        image->getData(), libgfx_getFramePointer(frameset, 0),
        image->getByteCount()
    );

    return image;
  }

  private:
  String mAssetName;
};

static void parseObjFace(
    const String &el, int *vertexIndex, int *texCoordsIndex, int *normalIndex
) {
  std::vector<String> parts;
  el.split('/', std::back_inserter(parts));

  if (parts[0].getLength() > 0) {
    *vertexIndex = atoi(parts[0].c_str()) - 1;
  }

  if (parts[1].getLength() > 0) {
    *texCoordsIndex = atoi(parts[1].c_str()) - 1;
  }

  if (parts[2].getLength() > 0) {
    *normalIndex = atoi(parts[2].c_str()) - 1;
  }
}

class ObjLoader {
  public:
  ObjLoader(const String &assetName) : mAssetName(assetName) {}

  MeshPtr operator()() {
    auto reader = ResourceReader::make(mAssetName.c_str());
    std::vector<uint8_t> buffer(reader->getSize());
    reader->read(buffer.data(), 1, buffer.size());
    String contents(reinterpret_cast<char *>(buffer.data()), buffer.size());

    std::vector<String> lines;
    // split() is very inefficient on the N64!
    contents.split('\n', std::back_inserter(lines));

    auto mesh = std::make_shared<Mesh>();

    std::vector<Vector3f> vertexCache;
    std::vector<Vector2f> texCoordsCache;
    std::vector<Vector3f> normalCache;

    for (auto &&line : lines) {
      if (line.getLength() < 1 || line[0] == '#') {
        continue;
      }

      std::vector<String> parts;
      line.split(' ', std::back_inserter(parts));

      if (parts[0] == "o") {
        logDebug("object");
      } else if (parts[0] == "v") {
        vertexCache.emplace_back(Vector3f(
            atof(parts[1].c_str()), atof(parts[2].c_str()),
            atof(parts[3].c_str())
        ));
      } else if (parts[0] == "vt") {
        texCoordsCache.emplace_back(
            Vector2f(atof(parts[1].c_str()), atof(parts[2].c_str()))
        );
      } else if (parts[0] == "vn") {
        normalCache.emplace_back(Vector3f(
            atof(parts[1].c_str()), atof(parts[2].c_str()),
            atof(parts[3].c_str())
        ));
      } else if (parts[0] == "s") {
        logDebug("s");
      } else if (parts[0] == "f") {
        uint32_t meshVertexIndex = mesh->getVertices().size();

        for (int i = 0; i < 3; ++i) {
          int vertexIndex;
          int texCoordsIndex;
          int normalIndex;

          parseObjFace(
              parts[i + 1], &vertexIndex, &texCoordsIndex, &normalIndex
          );

          mesh->getVertices().emplace_back(vertexCache[vertexIndex]);
          mesh->getTexCoords().emplace_back(texCoordsCache[vertexIndex]);
          mesh->getNormals().emplace_back(normalCache[normalIndex]);
        }

        mesh->getFaces().push_back(
            {meshVertexIndex, meshVertexIndex + 1, meshVertexIndex + 2}
        );
      } else {
        logDebug("unknown command {}", parts[0]);
      }
    }

    return mesh;
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
  const auto roomHalfWidth = 10.0f;
  const auto roomHalfLength = 10.0f;
  const auto roomHeight = 3.0f;

  auto mesh = make_shared<Mesh>();

  // floor
  MeshBuilder::addQuadFace(
      mesh, {{
                {-roomHalfWidth, 0.0f, -roomHalfLength},
                {roomHalfWidth, 0.0f, -roomHalfLength},
                {-roomHalfWidth, 0.0f, roomHalfLength},
                {roomHalfWidth, 0.0f, roomHalfLength},
            }}
  );

  // front wall
  MeshBuilder::addQuadFace(
      mesh, {{
                {-roomHalfWidth, roomHeight, -roomHalfLength},
                {roomHalfWidth, roomHeight, -roomHalfLength},
                {-roomHalfWidth, 0.0f, -roomHalfLength},
                {roomHalfWidth, 0.0f, -roomHalfLength},
            }}
  );

  // left wall
  MeshBuilder::addQuadFace(
      mesh, {{
                {-roomHalfWidth, roomHeight, roomHalfLength},
                {-roomHalfWidth, roomHeight, -roomHalfLength},
                {-roomHalfWidth, 0.0f, roomHalfLength},
                {-roomHalfWidth, 0.0f, -roomHalfLength},
            }}
  );

  // left wall
  MeshBuilder::addQuadFace(
      mesh, {{
                {roomHalfWidth, roomHeight, -roomHalfLength},
                {roomHalfWidth, roomHeight, roomHalfLength},
                {roomHalfWidth, 0.0f, -roomHalfLength},
                {roomHalfWidth, 0.0f, roomHalfLength},
            }}
  );

  // back wall
  MeshBuilder::addQuadFace(
      mesh, {{
                {roomHalfWidth, roomHeight, roomHalfLength},
                {-roomHalfWidth, roomHeight, roomHalfLength},
                {roomHalfWidth, 0.0f, roomHalfLength},
                {-roomHalfWidth, 0.0f, roomHalfLength},
            }}
  );

  for (size_t i = 0; i < 4; ++i) {
    mesh->getTexCoords()[i].x *= 4.f;
    mesh->getTexCoords()[i].y *= 4.f;
  }

  for (size_t i = 4; i < mesh->getTexCoords().size(); ++i) {
    mesh->getTexCoords()[i].x *= 4.f;
    mesh->getTexCoords()[i].y *= 2.f;
  }

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
  float direction = 0.f;

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

    auto diffuse =
        make_shared<Resource<Image>>(GfxImageLoader("wall_32x32.gfx"));

    auto model = stage->createModel();
    model->setMesh(makeMesh());
    model->getMaterial().setDiffuse(diffuse);

    auto pillarResource = make_shared<Resource<Mesh>>(ObjLoader("pillar.obj"));
    auto pillarMesh = pillarResource->get().get();

    // having just two pillars (216 triangles each) already causes serious lag on N64
    for (float z = -8.0f; z <= 8.0f; z += 4.0f) {
      // left side
      auto pillarModel = stage->createModel();
      pillarModel->setMesh(pillarMesh);
      pillarModel->getTransform() =
          pillarModel->getTransform().translate({-8.0f, 0.0f, z});
      pillarModel->getMaterial().setDiffuse(diffuse);

      // right side
      pillarModel = stage->createModel();
      pillarModel->setMesh(pillarMesh);
      pillarModel->getTransform() =
          pillarModel->getTransform().translate({8.0f, 0.0f, z});
      pillarModel->getMaterial().setDiffuse(diffuse);
    }
  });

  app.addInterval(60, [&](float elapsedTime) {
    if (elapsedTime > 1.0f) {
      elapsedTime = 1.0f;
    }

    t += elapsedTime;

    inputManager.update(&canvas->getButtonEvents(), elapsedTime);

    float speed = 6.0f;
    float forwardFactor = std::cos(direction);
    float sidewardFactor = std::sin(direction);

    // if (inputManager.isButtonHeld("fast")) {
    //   speed = 4.0f;
    // }

    if (inputManager.isButtonHeld("up")) {
      camera.setPosition(
          camera.getPosition() + Vector3f::up() * elapsedTime * speed
      );
    }

    if (inputManager.isButtonHeld("down")) {
      camera.setPosition(
          camera.getPosition() + Vector3f::down() * elapsedTime * speed
      );
    }

    if (inputManager.isButtonHeld("forward")) {
      camera.setPosition(
          camera.getPosition() + (Vector3f::forward() * forwardFactor +
                                  Vector3f::right() * sidewardFactor) *
                                     elapsedTime * speed
      );
    }

    if (inputManager.isButtonHeld("backward")) {
      camera.setPosition(
          camera.getPosition() + (Vector3f::backward() * forwardFactor +
                                  Vector3f::left() * sidewardFactor) *
                                     elapsedTime * speed
      );
    }

    if (inputManager.isButtonHeld("left")) {
      camera.setPosition(
          camera.getPosition() + (Vector3f::left() * forwardFactor +
                                  Vector3f::forward() * sidewardFactor) *
                                     elapsedTime * speed
      );
    }

    if (inputManager.isButtonHeld("right")) {
      camera.setPosition(
          camera.getPosition() + (Vector3f::right() * forwardFactor +
                                  Vector3f::backward() * sidewardFactor) *
                                     elapsedTime * speed
      );
    }

    if (inputManager.isButtonHeld("turnLeft")) {
      direction -= elapsedTime * 4.f;
    }

    if (inputManager.isButtonHeld("turnRight")) {
      direction += elapsedTime * 4.f;
    }

    camera.resetRotation();
    camera.rotateY(direction);

    pointLight->position = {sin(t) * 5.0f, 1.0f, cos(t) * 5.0f};

    canvas->setCamera3d(camera);
  });

  return app.run();
}
