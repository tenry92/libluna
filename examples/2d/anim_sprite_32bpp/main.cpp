#include <cstring>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>
#include <libluna/Filesystem/FileReader.hpp>
#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>
#include <libluna/Texture.hpp>
#include <libluna/Vector.hpp>

#include "../../common.hpp"

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

#define NUM_FRAMES 23
#define TEX_BASE_SLOT_32BPP 0
#define TEX_BASE_SLOT_24BPP (TEX_BASE_SLOT_32BPP + NUM_FRAMES)
#define TEX_BASE_SLOT_16BPP (TEX_BASE_SLOT_24BPP + NUM_FRAMES)

using namespace std;
using namespace Luna;
using namespace Luna::Filesystem;

#ifdef __SWITCH__
static const char* assetsPath = "romfs:/assets";
#elif defined(N64)
static const char* assetsPath = "rom:/assets";
#else
static const char* assetsPath = "data/assets";
#endif

class ExampleApp : public Application {
  public:
  using Application::Application;

  protected:
  void init() override {
    mCanvas = allocCanvas();
    mCanvas->setDisplayMode({
      Vector2i{CANVAS_WIDTH, CANVAS_HEIGHT}, // resolution
      false,                                 // fullscreen
      getDefaultVideoDriver()
    });
    mCanvas->setBackgroundColor(ColorRgb{0.f, 0.5f, 1.f});

    mCamera.setStage(&mStage);
    mCanvas->setCamera2d(mCamera);
  }

  void update(float deltaTime) override {
    mTime += deltaTime;

    if (!mTexturesLoaded) {
      loadTextures();

      mSprite32bpp = mStage.allocSprite();
      mSprite32bpp->setPosition({64.f, 64.f});

      mSprite24bpp = mStage.allocSprite();
      mSprite24bpp->setPosition({128.f, 64.f});

      mSprite16bpp = mStage.allocSprite();
      mSprite16bpp->setPosition({192.f, 64.f});

      mTexturesLoaded = true;
    }

    int frameIndex = static_cast<int>(mTime * 24.0) % NUM_FRAMES;
    mSprite32bpp->setTexture(TEX_BASE_SLOT_32BPP + frameIndex);
    mSprite24bpp->setTexture(TEX_BASE_SLOT_24BPP + frameIndex);
    mSprite16bpp->setTexture(TEX_BASE_SLOT_16BPP + frameIndex);
  }

  void handleButtonEvent(const ButtonEvent& event) override {}

  private:
  void loadTextures() {
    loadTexture(
      getAssetsPath().cd("coin_32bpp.bmp"), mTexture32bpp
    );
    loadTexture(
      getAssetsPath().cd("coin_24bpp.bmp"), mTexture24bpp
    );
    loadTexture(
      getAssetsPath().cd("coin_16bpp.bmp"), mTexture16bpp
    );

    for (int i = 0; i < NUM_FRAMES; i++) {
      mTexture32bppFrames[i] = mTexture32bpp.crop(
        Vector2i{32, 32}, Vector2i{32 * i, 0}
      );
      mCanvas->uploadTexture(TEX_BASE_SLOT_32BPP + i, &mTexture32bppFrames[i]);

      mTexture24bppFrames[i] = mTexture24bpp.crop(
        Vector2i{32, 32}, Vector2i{32 * i, 0}
      );
      mCanvas->uploadTexture(TEX_BASE_SLOT_24BPP + i, &mTexture24bppFrames[i]);

      mTexture16bppFrames[i] = mTexture16bpp.crop(
        Vector2i{32, 32}, Vector2i{32 * i, 0}
      );
      mCanvas->uploadTexture(TEX_BASE_SLOT_16BPP + i, &mTexture16bppFrames[i]);
    }
  }

  bool mTexturesLoaded{false};
  Canvas* mCanvas{nullptr};
  Stage mStage;
  Camera2d mCamera;
  Texture mTexture32bpp;
  Texture mTexture24bpp;
  Texture mTexture16bpp;
  Texture mTexture32bppFrames[NUM_FRAMES];
  Texture mTexture24bppFrames[NUM_FRAMES];
  Texture mTexture16bppFrames[NUM_FRAMES];
  Sprite* mSprite32bpp{nullptr};
  Sprite* mSprite24bpp{nullptr};
  Sprite* mSprite16bpp{nullptr};
  double mTime{0.f};
};

int main(int argc, char** argv) {
  ExampleApp app(argc, argv);
  app.setAssetsPath(assetsPath);

  return app.run();
}
