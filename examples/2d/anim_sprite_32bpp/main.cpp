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

template<typename T>
class DiscreteAnimation {
  public:
  DiscreteAnimation() {}
  DiscreteAnimation(std::vector<T> &&frames, float frameRate) : mFrames(std::move(frames)), mFrameRate(frameRate) {}

  T &advance(float seconds) {
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

class GfxImageLoader {
  public:
  GfxImageLoader(libgfx_Gfx *gfx, int frameIndex) : mGfx(gfx), mFrameIndex(frameIndex) {}

  Image operator()() {
    auto frameset = &mGfx->framesets[0];

    auto image = Image(32, {frameset->width, frameset->height});
    memcpy(image.getData(), libgfx_getFramePointer(frameset, mFrameIndex), image.getByteCount());

    return image;
  }

  private:
  libgfx_Gfx *mGfx;
  int mFrameIndex;
};

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Canvas> canvas;
  Sprite *sprite;
  ResourceReaderPtr reader;
  DiscreteAnimation<Image> animation;

  app.whenReady([&]() {
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    canvas->setBackgroundColor({0.0f, 0.9f, 0.6f, 1.0f});

    reader = ResourceReader::make("coin.gfx");
    auto gfx = libgfx_loadImageFromCallback(readFromResource, reader.get());
    auto frameset = &gfx->framesets[0];

    std::vector<Image> frames;
    frames.reserve(frameset->numFrames);

    for (int i = 0; i < frameset->numFrames; ++i) {
      frames.emplace_back(GfxImageLoader(gfx, i)());
    }

    animation = DiscreteAnimation<Image>(std::move(frames), 30.0f);

    auto stage = make_shared<Stage>();
    sprite = stage->createSprite();
    sprite->setPosition({CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2});
    canvas->setStage(stage);
  });

  app.addInterval(60, [&](float elapsedTime) {
    sprite->setImage(&animation.advance(elapsedTime));
  });

  return app.run();
}
