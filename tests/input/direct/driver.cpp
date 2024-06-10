#include <iostream>

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

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Canvas> canvas;

  app.whenReady([&]() {
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
  });

  app.addInterval(60, [&](float elapsedTime) {
    auto &events = canvas->getButtonEvents();

    while (!events.empty()) {
      cout << events.size() << endl;
      auto event = events.front();
      events.pop();

      if (event.isDown()) {
        cout << event.getName().s_str() << " pressed" << endl;
      } else {
        cout << event.getName().s_str() << " released" << endl;
      }
    }
  });

  return app.run();
}
