#include <vector>
#include <cmath>
#include <cstring>

#include <libgfx/libgfx.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>

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

class ResourceIoStream : public Assimp::IOStream {
  friend class ResourceIoSystem;
  ResourceReaderPtr mReader;
  String mName;

  protected:
  ResourceIoStream(const String &name) : mName{name} {
    mReader = ResourceReader::make(name.c_str());
  }

  public:
  ~ResourceIoStream() = default;

  size_t Read(void* buffer, size_t size, size_t count) override {
    return mReader->read(reinterpret_cast<uint8_t *>(buffer), size, count);
  }

  size_t Write(const void* buffer, size_t size, size_t count) override {
    return 0;
  }

  aiReturn Seek(size_t offset, aiOrigin origin) override {
    switch (origin) {
      case aiOrigin_SET:
        mReader->seek(offset);
        return aiReturn_SUCCESS;
      case aiOrigin_CUR:
        mReader->seekRelative(offset);
        return aiReturn_SUCCESS;
      case aiOrigin_END:
        mReader->seek(mReader->getSize() + offset);
        return aiReturn_SUCCESS;
    }

    return aiReturn_FAILURE;
  }

  size_t Tell() const override {
    return mReader->tell();
  }

  size_t FileSize() const override {
    return mReader->getSize();
  }

  void Flush () {}
};

class ResourceIoSystem : public Assimp::IOSystem {
  public:
  ResourceIoSystem()  = default;
  ~ResourceIoSystem() = default;

  bool Exists(const char *name) const override {
    return true;
  }

  char getOsSeparator() const override {
    return '/';
  }

  Assimp::IOStream *Open(const char *name, [[maybe_unused]] const char *mode) override {
    return new ResourceIoStream(name);
  }

  void Close(Assimp::IOStream* file) override {
    delete file;
  }
};

class DummyImageLoader {
  public:
  DummyImageLoader(String filename) : mFilename(filename) {}

  ImagePtr operator()() {
    auto reader = ResourceReader::make(mFilename.c_str());
    auto gfx = libgfx_loadImageFromCallback(readFromResource, reader.get());
    auto frameset = &gfx->framesets[0];
    auto image = Image::makeRgb32({frameset->width, frameset->height});
    memcpy(image->getData(), frameset->data, image->getByteCount());
    libgfx_freeGfx(gfx);

    return image;
  }

  private:
  String mFilename;
};

Vector2f toVector2f(const aiVector3D &other) {
  return Vector2f(other.x, other.y);
}

Vector3f toVector3f(const aiVector3D &other) {
  return Vector3f(other.x, other.y, other.z);
}

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

  app.whenReady([&]() {
    Assimp::Importer importer;
    importer.SetIOHandler(new ResourceIoSystem());
    const aiScene *scene = importer.ReadFile("dungeon.gltf",
      aiProcess_CalcTangentSpace |
      aiProcess_Triangulate |
      aiProcess_JoinIdenticalVertices |
      aiProcess_SortByPType
    );

    for (int i = 0; i < scene->mNumMeshes; ++i) {
      auto m = scene->mMeshes[i];

      mesh->getVertices().reserve(m->mNumVertices);
      mesh->getNormals().reserve(m->mNumVertices);
      mesh->getTangents().reserve(m->mNumVertices);
      mesh->getBitangents().reserve(m->mNumVertices);
      mesh->getTexCoords().reserve(m->mNumVertices);
      mesh->getFaces().reserve(m->mNumFaces);

      for (int vi = 0; vi < m->mNumVertices; ++vi) {
        mesh->getVertices().emplace_back(toVector3f(m->mVertices[vi])); // aiVector3D
        mesh->getNormals().emplace_back(toVector3f(m->mNormals[vi])); // aiVector3D
        mesh->getTangents().emplace_back(toVector3f(m->mTangents[vi])); // aiVector3D
        mesh->getBitangents().emplace_back(toVector3f(m->mBitangents[vi])); // aiVector3D
        mesh->getTexCoords().emplace_back(toVector2f(m->mTextureCoords[0][vi])); // aiVector3D
      }

      for (int fi = 0; fi < m->mNumFaces; ++fi) {
        auto face = m->mFaces[fi];
        mesh->getFaces().push_back({
          face.mIndices[0], face.mIndices[1], face.mIndices[2]
        });
      }

      break;
    }


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

    model = make_shared<Model>();
    stage->add(model);
    model->setMesh(mesh);

    auto diffuseRef = make_shared<Resource<Image>>(DummyImageLoader("textures/wall_32x32.gfx"));
    model->getMaterial().setDiffuse(diffuseRef);

    // auto diffuseRef = make_shared<Resource<Image>>(DummyImageLoader("textures/BricksDragfacedRunning008_COL_1K.gfx"));
    // model->getMaterial().setDiffuse(diffuseRef);

    // auto normalRef = make_shared<Resource<Image>>(DummyImageLoader("textures/BricksDragfacedRunning008_NRM_1K.gfx"));
    // model->getMaterial().setNormal(normalRef);
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
