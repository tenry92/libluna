#include <cmath>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>
#include <libluna/Math.hpp>

#ifdef __SWITCH__
#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#else
#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600
#endif

using namespace std;
using namespace Luna;

class ExampleApp : public Application {
  public:
  using Application::Application;

  protected:
  void init() override {
    mCanvas = createCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    mCanvas->setDisplayMode({
      Vector2i{CANVAS_WIDTH, CANVAS_HEIGHT}, // resolution
      false,                                 // fullscreen
      getDefaultVideoDriver()
    });
    mCanvas->setBackgroundColor(ColorRgb{0.f, 0.f, 0.f});
  }

  void update(float deltaTime) override {
    mTime += deltaTime * 0.4f;
    ColorRgb baseColor;

    switch (static_cast<int>(mTime) % 6) {
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

    float lightness = std::sin(Math::kPi * std::fmod(mTime, 1.0f));

    baseColor.red *= lightness;
    baseColor.green *= lightness;
    baseColor.blue *= lightness;
    mCanvas->setBackgroundColor(baseColor);
  }

  void handleButtonEvent(const ButtonEvent& event) override {}

  private:
  Canvas* mCanvas{nullptr};
  double mTime{0.f};
};

int main(int argc, char **argv) {
  ExampleApp app(argc, argv);

  return app.run();
}
