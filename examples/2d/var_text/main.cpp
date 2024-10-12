#include <cstring>

#include <libgfx/libgfx.h>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>
#include <libluna/ResourceReader.hpp>

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

using namespace std;
using namespace Luna;

static int readFromResource(void *dest, size_t size, void *userData) {
  auto *reader = reinterpret_cast<ResourceReader *>(userData);

  return reader->read(reinterpret_cast<uint8_t *>(dest), 1, size);
}

class GfxFontLoader {
  public:
  GfxFontLoader(const String &assetName) : mAssetName(assetName) {}

  FontPtr operator()() {
    auto reader = ResourceReader::make(mAssetName.c_str());
    auto gfx = libgfx_loadImageFromCallback(readFromResource, reader.get());

    auto font = Font::make();
    font->setBaseLine(gfx->font.baseLine);
    font->setLineHeight(gfx->font.lineHeight);

    for (int i = 0; i < gfx->numCharacters; ++i) {
      auto gfxCh = &gfx->characters[i];
      auto frameset = &gfx->framesets[gfxCh->frameIndex];

      auto ch = font->makeCharForCodePoint(gfxCh->codePoint);

      if (frameset->width > 0 && frameset->height > 0) {
        ch->image = Image(32, {frameset->width, frameset->height});
        memcpy(ch->image.getData(), frameset->data, ch->image.getByteCount());
      }

      ch->offset = Vector2i(gfxCh->xOffset, gfxCh->yOffset);
      ch->advance = gfxCh->advance;
    }

    libgfx_freeGfx(gfx);

    return font;
  }

  private:
  String mAssetName;
};

int main(int argc, char **argv) {
  Application app(argc, argv);

  shared_ptr<Canvas> canvas;

  app.whenReady([&]() {
    canvas = app.makeCanvas({CANVAS_WIDTH, CANVAS_HEIGHT});
    canvas->setVideoDriver(app.getDefaultVideoDriver());
    canvas->setBackgroundColor({0.0f, 0.9f, 0.6f, 1.0f});

    auto stage = make_shared<Stage>();
    auto text = stage->createText();
    text->setFont(make_shared<Resource<Font>>(GfxFontLoader("font.gfx")));
    text->setContent("Welcome text!\nThis is a demonstration\nfor rendering text!");
    canvas->setStage(stage);
  });

  return app.run();
}
