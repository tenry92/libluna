#include <cstring>

#include <libluna/Application.hpp>
#include <libluna/Color.hpp>
#include <libluna/Filesystem/FileReader.hpp>
#include <libluna/Logger.hpp>
#include <libluna/ResourceReader.hpp>
#include <libluna/Texture.hpp>
#include <libluna/Vector.hpp>

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

static void swapRedBlueChannels(Texture& texture) {
  int width = texture.getWidth();
  int height = texture.getHeight();

  if (texture.getBitsPerPixel() == 16) {
#ifdef N64
    uint8_t* data = texture.getData();
    uint16_t* pixels = reinterpret_cast<uint16_t*>(texture.getRgb16());

    for (int i = 0; i < width * height; i++) {
      pixels[i] <<= 1;
      std::swap(data[i * 2 + 0], data[i * 2 + 1]);
    }
#else
    ColorRgb16* pixels = texture.getRgb16();

    for (int i = 0; i < width * height; i++) {
      ColorRgb16 tmp = pixels[i];
      pixels[i].red = tmp.blue;
      pixels[i].blue = tmp.red;
    }
#endif
  } else if (texture.getBitsPerPixel() == 24) {
    ColorRgb24* pixels = texture.getRgb24();

    for (int i = 0; i < width * height; i++) {
      std::swap(pixels[i].red, pixels[i].blue);
    }
  } else if (texture.getBitsPerPixel() == 32) {
    ColorRgb32* pixels = texture.getRgb32();

    for (int i = 0; i < width * height; i++) {
      std::swap(pixels[i].red, pixels[i].blue);
    }
  }
}

static void loadTexture(const Path& filename, Texture& texture) {
  logInfo("Loading texture from file: {}", String(filename).c_str());

  auto fileReader = FileReader::make(filename);

  // if (!fileReader || !fileReader->isValid()) {
  //   logError("Failed to open BMP file: {}", String(filename).c_str());
  //   return;
  // }

  // BMP file header (14 bytes)
  uint16_t signature;
  uint32_t fileSize;
  uint32_t reserved;
  uint32_t dataOffset;

  fileReader->read(&signature);
  fileReader->read(&fileSize);
  fileReader->read(&reserved);
  fileReader->read(&dataOffset);

  // Check BMP signature (0x4D42 = "BM")
  if (signature != 0x4D42) {
    logError("Invalid BMP file signature");
    return;
  }

  // DIB header (BITMAPINFOHEADER, 40 bytes minimum)
  uint32_t headerSize;
  int32_t width;
  int32_t height;
  uint16_t colorPlanes;
  uint16_t bitsPerPixel;
  uint32_t compression;
  uint32_t imageSize;
  int32_t xPixelsPerMeter;
  int32_t yPixelsPerMeter;
  uint32_t colorsUsed;
  uint32_t colorsImportant;

  fileReader->read(&headerSize);
  fileReader->read(&width);
  fileReader->read(&height);
  fileReader->read(&colorPlanes);
  fileReader->read(&bitsPerPixel);
  fileReader->read(&compression);
  fileReader->read(&imageSize);
  fileReader->read(&xPixelsPerMeter);
  fileReader->read(&yPixelsPerMeter);
  fileReader->read(&colorsUsed);
  fileReader->read(&colorsImportant);

  // Basic validation
  if (compression != 0 && compression != 3) { // No compression
    logError("Unsupported BMP compression ({})", compression);
    return;
  }

  if (bitsPerPixel != 16 && bitsPerPixel != 24 && bitsPerPixel != 32) {
    logError("Unsupported BMP bits per pixel ({})", bitsPerPixel);
    return;
  }

  // BMP stores images bottom-up, handle negative height
  bool bottomUp = height > 0;

  if (height < 0) {
    height = -height;
  }

  logInfo(
    "Loading BMP: {}x{}, {} bpp, bottom-up: {}",
    width, height, bitsPerPixel, bottomUp ? "yes" : "no"
  );

  // Create texture
  texture = Texture(static_cast<int>(bitsPerPixel), Vector2i{width, height});

  // Seek to pixel data
  fileReader->seek(dataOffset);

  // Calculate row padding (rows are aligned to 4-byte boundaries)
  int bytesPerPixel = bitsPerPixel / 8;
  int rowSize = width * bytesPerPixel;
  int padding = (4 - (rowSize % 4)) % 4;

  // Read pixel data
  for (int y = 0; y < height; y++) {
    int targetY = bottomUp ? (height - 1 - y) : y;

    if (bitsPerPixel == 16) {
      ColorRgb16* row = texture.getRgb16() + (targetY * width);
      // cast to uint8_t to avoid implicit endian conversion
      fileReader->read(reinterpret_cast<uint8_t*>(row), width * 2);
    } else if (bitsPerPixel == 24) {
      ColorRgb24* row = texture.getRgb24() + (targetY * width);
      // cast to uint8_t to avoid implicit endian conversion
      fileReader->read(reinterpret_cast<uint8_t*>(row), width * 3);
    } else if (bitsPerPixel == 32) {
      ColorRgb32* row = texture.getRgb32() + (targetY * width);
      // cast to uint8_t to avoid implicit endian conversion
      fileReader->read(reinterpret_cast<uint8_t*>(row), width * 4);
    }

    // Skip padding bytes
    if (padding > 0) {
      fileReader->skip(padding);
    }
  }

  swapRedBlueChannels(texture);
}

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
      // mSprite32bpp->setTexture(TEX_BASE_SLOT_32BPP);
      mSprite32bpp->setPosition({64.f, 64.f});

      mSprite24bpp = mStage.allocSprite();
      // mSprite24bpp->setTexture(TEX_BASE_SLOT_24BPP);
      mSprite24bpp->setPosition({128.f, 64.f});

      mSprite16bpp = mStage.allocSprite();
      // mSprite16bpp->setTexture(TEX_BASE_SLOT_16BPP);
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
