#include <vector>
#include <cmath>

#include <libluna/Application.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/Color.hpp>
#include <libluna/Util/Png.hpp>
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
using namespace Luna::Util;

class DummyImageLoader {
  public:
  DummyImageLoader() {}

  ImagePtr operator()() {
    int frameCount = 23;

    Png png(ResourceReader::make("coin.png"));
    logDebug("decode png file");
    return png.decode(frameCount);
  }
};

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Stage> stage;
  shared_ptr<Canvas> canvas;
  shared_ptr<Sprite> sprite;

  app.whenReady([&]() {
    stage = make_shared<Stage>();
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    logDebug("set video driver");
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    logDebug("set stage");
    canvas->setStage(stage);

    logDebug("make imageRef");
    auto imageRef = make_shared<ResourceRef<Image>>(DummyImageLoader());

    logDebug("make sprite");
    sprite = stage->makeSprite();
    sprite->setImage(imageRef);
    sprite->setPosition({CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2});
    sprite->setAnimationFrames(0, 23);
    sprite->setFrameRate(30.0f);
  });

  app.addInterval(60, [&](float elapsedTime) {
    sprite->advanceAnimation(elapsedTime);
  });

  return app.run();
}
