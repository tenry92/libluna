#include <vector>

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

  shared_ptr<Stage> stage;
  shared_ptr<Canvas> canvas;

  app.whenReady([&]() {
    stage = make_shared<Stage>();
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    canvas->setStage(stage);
  });

  return app.run();
}
