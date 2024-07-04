#include <vector>
#include <cmath>
#include <cstring>

#include <libgfx/libgfx.h>

#include <libluna/Application.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/Color.hpp>
#include <libluna/ResourceReader.hpp>
#include <libluna/Logger.hpp>

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

template<typename T>
class DiscreteAnimation {
  public:
  DiscreteAnimation() {}
  DiscreteAnimation(const std::vector<T> &frames, float frameRate) : mFrames(frames), mFrameRate(frameRate) {}

  T advance(float seconds) {
    mCurrentFrame = static_cast<float>(std::fmod(
      mCurrentFrame + mFrameRate * seconds, mFrames.size()
    ));

    return mFrames.at(static_cast<int>(mCurrentFrame));
  }

  private:
  std::vector<T> mFrames;
  float mFrameRate;
  float mCurrentFrame{0};
};

class DummyImageLoader {
  public:
  DummyImageLoader(libgfx_Image *gfx, int frameIndex) : mGfx(gfx), mFrameIndex(frameIndex) {}

  ImagePtr operator()() {
    auto image = Image::makeRgb32({mGfx->width, mGfx->height});
    memcpy(image->getData(), libgfx_getFramePointer(mGfx, mFrameIndex), image->getByteCount());

    return image;
  }

  private:
  libgfx_Image *mGfx;
  int mFrameIndex;
};

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Stage> stage;
  shared_ptr<Canvas> canvas;
  shared_ptr<Sprite> sprite;

  ResourceReaderPtr reader;
  DiscreteAnimation<ImageResPtr> animation;

  app.whenReady([&]() {
    stage = make_shared<Stage>();
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    logDebug("set video driver");
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    logDebug("set stage");
    canvas->setStage(stage);

    reader = ResourceReader::make("coin.gfx");
    auto gfx = libgfx_loadImageFromCallback(readFromResource, reader.get());

    logDebug("make imageRef");
    std::vector<ImageResPtr> frames;
    frames.reserve(gfx->numFrames);

    logDebug("{}x{}: {}", gfx->width, gfx->height, gfx->numFrames);

    for (int i = 0; i < gfx->numFrames; ++i) {
      logDebug("loading frame {}", i);
      frames.emplace_back(make_shared<Resource<Image>>(DummyImageLoader(gfx, i)));
    }

    animation = DiscreteAnimation<ImageResPtr>(frames, 30.0f);

    // libgfx_freeImage(gfx);

    logDebug("make sprite");
    sprite = stage->makeSprite();
    // sprite->setImage(image);
    sprite->setPosition({CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2});
    // sprite->setAnimationFrames(0, 23);
    // sprite->setFrameRate(30.0f);
  });

  app.addInterval(60, [&](float elapsedTime) {
    // sprite->advanceAnimation(elapsedTime);
    sprite->setImage(animation.advance(elapsedTime));
  });

  return app.run();
}
