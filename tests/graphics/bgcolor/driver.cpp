#include <cmath>

#include <libluna/Application.hpp>
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

constexpr double pi = 3.14159265358979323846;

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Canvas> canvas;
  double t = 0.f;

  app.whenReady([&]() {
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
  });

  app.addInterval(60, [&](float elapsedTime) {
    t += elapsedTime * 0.4f;
    ColorRgb baseColor;

    switch (static_cast<int>(t) % 6) {
      case 0:
        baseColor = ColorRgb{1.0f, 0.f, 0.f};
        break;
      case 1:
        baseColor = ColorRgb{1.0f, 1.f, 0.f};
        break;
      case 2:
        baseColor = ColorRgb{0.0f, 1.f, 0.f};
        break;
      case 3:
        baseColor = ColorRgb{0.0f, 1.f, 1.f};
        break;
      case 4:
        baseColor = ColorRgb{0.0f, 0.f, 1.f};
        break;
      case 5:
        baseColor = ColorRgb{1.0f, 0.f, 1.f};
        break;
    }

    float lightness = std::sin(pi * std::fmod(t, 1.0f));

    baseColor.red *= lightness;
    baseColor.green *= lightness;
    baseColor.blue *= lightness;
    canvas->setBackgroundColor(baseColor);
  });

  return app.run();
}
