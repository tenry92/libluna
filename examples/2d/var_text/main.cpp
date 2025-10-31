#include <cstring>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>
#include <libluna/Filesystem/FileReader.hpp>
#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>
#include <libluna/Texture.hpp>
#include <libluna/Vector.hpp>

#include "../../common.hpp"

extern "C" {
#include "bmfont.h"
}

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
    mCanvas = createCanvas();
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

      Text* mText = mStage.allocText();
      mText->setFont(&mFont);
      mText->setPosition(Vector2i{24, 24});
      mText->setContent("Welcome text!\nThis is a demonstration\nfor rendering text!");

      mTexturesLoaded = true;
    }
  }

  private:
  void loadTextures() {
    auto fileReader = FileReader::make(getAssetsPath().cd("font.fnt"));
    std::vector<char> fntText(fileReader->getSize() + 1);
    fileReader->read(fntText.data(), fileReader->getSize());
    fntText[fileReader->getSize()] = '\0';

    BMFont* bmFont = parseBMFont(fntText.data());

    mFont.setLineHeight(bmFont->common.lineHeight);
    mFont.setBaseLine(bmFont->common.base);

    mFontTextures.resize(bmFont->common.pages);

    for (int i = 0; i < bmFont->common.pages; ++i) {
      String pageFile = String(bmFont->pages[i].file);
      loadTexture(getAssetsPath().cd(pageFile), mFontTextures[i]);

      // due to N64 texture size limits, we'll upload each glyph as a separate
      // texture further below
#ifndef N64
      mCanvas->uploadTexture(i, &mFontTextures[i]);
#endif
    }

#ifdef N64
    mGlyphTextures.resize(bmFont->charCount);
#endif

    for (int i = 0; i < bmFont->charCount; ++i) {
      BMFontChar& bmChar = bmFont->chars[i];

      Font::Glyph* glyph = mFont.makeGlyphForCodePoint(static_cast<String::CodePoint>(bmChar.id));
#ifdef N64
      glyph->textureSlot = i;
#else
      glyph->textureSlot = bmChar.page;
#endif
      glyph->crop = Recti{
        bmChar.x,
        bmChar.y,
        bmChar.width,
        bmChar.height
      };
      glyph->offset = Vector2i{bmChar.xoffset, bmChar.yoffset};
      glyph->advance = bmChar.xadvance;

#ifdef N64
      mGlyphTextures[i] = mFontTextures[bmChar.page].crop(
        Vector2i{bmChar.width, bmChar.height},
        Vector2i{bmChar.x, bmChar.y}
      );
      mCanvas->uploadTexture(glyph->textureSlot, &mGlyphTextures[i]);
#endif
    }
  }

  bool mTexturesLoaded{false};
  Canvas* mCanvas{nullptr};
  Stage mStage;
  Camera2d mCamera;
  Font mFont;
  std::vector<Texture> mFontTextures;
#ifdef N64
  std::vector<Texture> mGlyphTextures;
#endif
  double mTime{0.f};
};

int main(int argc, char** argv) {
  ExampleApp app(argc, argv);
  app.setAssetsPath(assetsPath);

  return app.run();
}
