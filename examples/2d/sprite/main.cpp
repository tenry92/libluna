#include <cmath>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>
#include <libluna/Math.hpp>
#include <libluna/Texture.hpp>

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
    mCanvas = createCanvas();
    mCanvas->setDisplayMode({
      Vector2i{CANVAS_WIDTH, CANVAS_HEIGHT}, // resolution
      false,                                 // fullscreen
      getDefaultVideoDriver()
    });
    mCanvas->setBackgroundColor(ColorRgb{0.f, 0.f, 0.f});

    mCamera.setStage(&mStage);
    mCanvas->setCamera2d(mCamera);
  }

  void update(float deltaTime) override {
    mTime += deltaTime * 0.4f;

    if (!mTexturesLoaded) {
      loadTextures();

      mCanvas->uploadTexture(0, &mTexture32bpp);
      mCanvas->uploadTexture(1, &mTexture24bpp);
      mCanvas->uploadTexture(2, &mTexture16bpp);

      Sprite* sprite1 = mStage.allocSprite();
      sprite1->setTexture(0);
      sprite1->setPosition({64.f, 64.f});

      Sprite* sprite2 = mStage.allocSprite();
      sprite2->setTexture(1);
      sprite2->setPosition({128.f, 64.f});

      Sprite* sprite3 = mStage.allocSprite();
      sprite3->setTexture(2);
      sprite3->setPosition({192.f, 64.f});
    }

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

  private:
  void loadTextures() {
    generateTexture32bpp();
    generateTexture24bpp();
    generateTexture16bpp();
  }

  void generateTexture32bpp() {
    mTexture32bpp = Texture(32, Vector2i(32, 32));

    // Fill corners (8x8 pixels each)
    // Top left corner - red
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        mTexture32bpp.rgb32At(x, y) = ColorRgb32{255, 0, 0, 255};
      }
    }

    // Top right corner - green
    for (int y = 0; y < 8; y++) {
      for (int x = 24; x < 32; x++) {
        mTexture32bpp.rgb32At(x, y) = ColorRgb32{0, 255, 0, 255};
      }
    }

    // Bottom left corner - yellow
    for (int y = 24; y < 32; y++) {
      for (int x = 0; x < 8; x++) {
        mTexture32bpp.rgb32At(x, y) = ColorRgb32{255, 255, 0, 255};
      }
    }

    // Bottom right corner - blue
    for (int y = 24; y < 32; y++) {
      for (int x = 24; x < 32; x++) {
        mTexture32bpp.rgb32At(x, y) = ColorRgb32{0, 0, 255, 255};
      }
    }

    // Top row - white (overwrites corners)
    for (int x = 8; x < 24; x++) {
      for (int y = 0; y < 8; y++) {
        mTexture32bpp.rgb32At(x, y) = ColorRgb32{255, 255, 255, 255};
      }
    }

    // Left column - black (overwrites corners)
    for (int y = 8; y < 24; y++) {
      for (int x = 0; x < 8; x++) {
        mTexture32bpp.rgb32At(x, y) = ColorRgb32{0, 0, 0, 255};
      }
    }

    // Right column - cyan (overwrites corners)
    for (int y = 8; y < 24; y++) {
      for (int x = 24; x < 32; x++) {
        mTexture32bpp.rgb32At(x, y) = ColorRgb32{0, 255, 255, 255};
      }
    }

    // Bottom row - purple (overwrites corners)
    for (int x = 8; x < 24; x++) {
      for (int y = 24; y < 32; y++) {
        mTexture32bpp.rgb32At(x, y) = ColorRgb32{255, 0, 255, 255};
      }
    }

    // Middle area (16x16 pixels from 8,8 to 23,23) - black with alpha gradient
    for (int y = 8; y < 24; y++) {
      for (int x = 8; x < 24; x++) {
        // Alpha gradient from fully opaque (255) at top to transparent (0) at bottom
        uint8_t alpha = static_cast<uint8_t>(255 - ((y - 8) * 255 / 15));
        mTexture32bpp.rgb32At(x, y) = ColorRgb32{0, 0, 0, alpha};
      }
    }

    mTexturesLoaded = true;
  }

  void generateTexture24bpp() {
    mTexture24bpp = Texture(24, Vector2i(32, 32));

    // Fill corners (8x8 pixels each)
    // Top left corner - red
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        mTexture24bpp.rgb24At(x, y) = ColorRgb24{255, 0, 0};
      }
    }

    // Top right corner - green
    for (int y = 0; y < 8; y++) {
      for (int x = 24; x < 32; x++) {
        mTexture24bpp.rgb24At(x, y) = ColorRgb24{0, 255, 0};
      }
    }

    // Bottom left corner - yellow
    for (int y = 24; y < 32; y++) {
      for (int x = 0; x < 8; x++) {
        mTexture24bpp.rgb24At(x, y) = ColorRgb24{255, 255, 0};
      }
    }

    // Bottom right corner - blue
    for (int y = 24; y < 32; y++) {
      for (int x = 24; x < 32; x++) {
        mTexture24bpp.rgb24At(x, y) = ColorRgb24{0, 0, 255};
      }
    }

    // Top row - white (overwrites corners)
    for (int x = 8; x < 24; x++) {
      for (int y = 0; y < 8; y++) {
        mTexture24bpp.rgb24At(x, y) = ColorRgb24{255, 255, 255};
      }
    }

    // Left column - black (overwrites corners)
    for (int y = 8; y < 24; y++) {
      for (int x = 0; x < 8; x++) {
        mTexture24bpp.rgb24At(x, y) = ColorRgb24{0, 0, 0};
      }
    }

    // Right column - cyan (overwrites corners)
    for (int y = 8; y < 24; y++) {
      for (int x = 24; x < 32; x++) {
        mTexture24bpp.rgb24At(x, y) = ColorRgb24{0, 255, 255};
      }
    }

    // Bottom row - purple (overwrites corners)
    for (int x = 8; x < 24; x++) {
      for (int y = 24; y < 32; y++) {
        mTexture24bpp.rgb24At(x, y) = ColorRgb24{255, 0, 255};
      }
    }

    // Middle area (16x16 pixels from 8,8 to 23,23) - black with alpha gradient
    for (int y = 8; y < 24; y++) {
      for (int x = 8; x < 24; x++) {
        uint8_t value = static_cast<uint8_t>(255 - ((y - 8) * 255 / 15));
        mTexture24bpp.rgb24At(x, y) = ColorRgb24{value, value, value};
      }
    }

    mTexturesLoaded = true;
  }

  void generateTexture16bpp() {
    mTexture16bpp = Texture(16, Vector2i(32, 32));

    // Fill corners (8x8 pixels each)
    // Top left corner - red
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        mTexture16bpp.rgb16At(x, y) = ColorRgb16{31, 0, 0, 1};
      }
    }

    // Top right corner - green
    for (int y = 0; y < 8; y++) {
      for (int x = 24; x < 32; x++) {
        mTexture16bpp.rgb16At(x, y) = ColorRgb16{0, 31, 0, 1};
      }
    }

    // Bottom left corner - yellow
    for (int y = 24; y < 32; y++) {
      for (int x = 0; x < 8; x++) {
        mTexture16bpp.rgb16At(x, y) = ColorRgb16{31, 31, 0, 1};
      }
    }

    // Bottom right corner - blue
    for (int y = 24; y < 32; y++) {
      for (int x = 24; x < 32; x++) {
        mTexture16bpp.rgb16At(x, y) = ColorRgb16{0, 0, 31, 1};
      }
    }

    // Top row - white (overwrites corners)
    for (int x = 8; x < 24; x++) {
      for (int y = 0; y < 8; y++) {
        mTexture16bpp.rgb16At(x, y) = ColorRgb16{31, 31, 31, 1};
      }
    }

    // Left column - black (overwrites corners)
    for (int y = 8; y < 24; y++) {
      for (int x = 0; x < 8; x++) {
        mTexture16bpp.rgb16At(x, y) = ColorRgb16{0, 0, 0, 1};
      }
    }

    // Right column - cyan (overwrites corners)
    for (int y = 8; y < 24; y++) {
      for (int x = 24; x < 32; x++) {
        mTexture16bpp.rgb16At(x, y) = ColorRgb16{0, 31, 31, 1};
      }
    }

    // Bottom row - purple (overwrites corners)
    for (int x = 8; x < 24; x++) {
      for (int y = 24; y < 32; y++) {
        mTexture16bpp.rgb16At(x, y) = ColorRgb16{31, 0, 31, 1};
      }
    }

    // Middle area (16x16 pixels from 8,8 to 23,23) - black with alpha gradient
    for (int y = 8; y < 24; y++) {
      for (int x = 8; x < 24; x++) {
        mTexture16bpp.rgb16At(x, y) = ColorRgb16{0, 0, 0, y < 16};
      }
    }

    mTexturesLoaded = true;
  }

  bool mTexturesLoaded{false};
  Canvas* mCanvas{nullptr};
  Stage mStage;
  Camera2d mCamera;
  Texture mTexture32bpp;
  Texture mTexture24bpp;
  Texture mTexture16bpp;
  double mTime{0.f};
};

int main(int argc, char** argv) {
  ExampleApp app(argc, argv);

  return app.run();
}
