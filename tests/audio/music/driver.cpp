#include <vector>
#include <cmath>

#include <libluna/Application.hpp>

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

  shared_ptr<Canvas> canvas;

  app.whenReady([&]() {
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());

    auto layer1 = std::make_shared<StreamAudioNode>("beat-layer1.opus");
    app.getAudioDestinationNode()->addInput(layer1);

    auto layer2 = std::make_shared<StreamAudioNode>("beat-layer2.opus");
    app.getAudioDestinationNode()->addInput(layer2);

    auto layer3 = std::make_shared<StreamAudioNode>("beat-layer3.opus");
    app.getAudioDestinationNode()->addInput(layer3);
  });

  return app.run();
}
