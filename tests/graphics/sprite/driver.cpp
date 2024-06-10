#include <vector>
#include <cmath>

#include <libluna/Application.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/Color.hpp>

#ifdef __SWITCH__
#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#else
#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600
#endif

using namespace std;
using namespace Luna;

class DummyImageLoader {
  public:
  DummyImageLoader() {}

  std::shared_ptr<Image> operator()() {
    int width = 16;
    int height = 16;
    int frameCount = 1;
    auto image = std::make_shared<Image>(32, Vector2i(width, height), frameCount);

    uint8_t frame[width * height * 4];

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        uint8_t *pixelPtr = frame + (x + y * width) * 4;
        pixelPtr[0] = 255; // red
        pixelPtr[1] = x * 255 / width; // green
        pixelPtr[2] = y * 255 / height; // blue
        pixelPtr[3] = 255; // alpha
      }
    }

    image->setFrameData(0, frame);

    return image;
  }
};

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Stage> stage;
  shared_ptr<Canvas> canvas;
  shared_ptr<Sprite> sprite;
  float t = 0.f;

  app.whenReady([&]() {
    stage = make_shared<Stage>();
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    canvas->setStage(stage);

    auto imageRef = make_shared<ResourceRef<Image>>(DummyImageLoader());

    sprite = stage->makeSprite();
    sprite->setImage(imageRef);
    sprite->setPosition({32, 128});
  });

  app.addInterval(60, [&](float elapsedTime) {
    t += elapsedTime;

    sprite->setPosition({
      CANVAS_WIDTH / 2 + cos(t) * CANVAS_WIDTH / 4,
      128
    });
  });

  return app.run();
}
