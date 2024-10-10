#include <cstring>

#include <libgfx/libgfx.h>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>
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

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Canvas> canvas;

  app.whenReady([&]() {
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    canvas->setBackgroundColor({0.0f, 0.9f, 0.6f, 1.0f});

    auto stage = make_shared<Stage>();
    auto sprite = stage->createSprite();
    sprite->setPosition({CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2});
    sprite->setImage(make_shared<Resource<Image>>(GfxImageLoader("color-test.gfx")));
    canvas->setStage(stage);
  });

  return app.run();
}
