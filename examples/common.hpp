#pragma once

#include <libluna/Color.hpp>
#include <libluna/Filesystem/FileReader.hpp>
#include <libluna/Logger.hpp>
#include <libluna/Texture.hpp>
#include <libluna/Vector.hpp>

static void swapRedBlueChannels(Luna::Texture& texture) {
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
    Luna::ColorRgb16* pixels = texture.getRgb16();

    for (int i = 0; i < width * height; i++) {
      Luna::ColorRgb16 tmp = pixels[i];
      pixels[i].red = tmp.blue;
      pixels[i].blue = tmp.red;
    }
#endif
  } else if (texture.getBitsPerPixel() == 24) {
    Luna::ColorRgb24* pixels = texture.getRgb24();

    for (int i = 0; i < width * height; i++) {
      std::swap(pixels[i].red, pixels[i].blue);
    }
  } else if (texture.getBitsPerPixel() == 32) {
    Luna::ColorRgb32* pixels = texture.getRgb32();

    for (int i = 0; i < width * height; i++) {
      std::swap(pixels[i].red, pixels[i].blue);
    }
  }
}

static void loadTexture(const Luna::Filesystem::Path& filename, Luna::Texture& texture) {
  Luna::logInfo("Loading texture from file: {}", Luna::String(filename).c_str());

  auto fileReader = Luna::Filesystem::FileReader::make(filename);

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
    Luna::logError("Invalid BMP file signature");
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
    Luna::logError("Unsupported BMP compression ({})", compression);
    return;
  }

  if (bitsPerPixel != 16 && bitsPerPixel != 24 && bitsPerPixel != 32) {
    Luna::logError("Unsupported BMP bits per pixel ({})", bitsPerPixel);
    return;
  }

  // BMP stores images bottom-up, handle negative height
  bool bottomUp = height > 0;

  if (height < 0) {
    height = -height;
  }

  Luna::logInfo(
    "Loading BMP: {}x{}, {} bpp, bottom-up: {}",
    width, height, bitsPerPixel, bottomUp ? "yes" : "no"
  );

  // Create texture
  texture = Luna::Texture(static_cast<int>(bitsPerPixel), Luna::Vector2i{width, height});

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
      Luna::ColorRgb16* row = texture.getRgb16() + (targetY * width);
      // cast to uint8_t to avoid implicit endian conversion
      fileReader->read(reinterpret_cast<uint8_t*>(row), width * 2);
    } else if (bitsPerPixel == 24) {
      Luna::ColorRgb24* row = texture.getRgb24() + (targetY * width);
      // cast to uint8_t to avoid implicit endian conversion
      fileReader->read(reinterpret_cast<uint8_t*>(row), width * 3);
    } else if (bitsPerPixel == 32) {
      Luna::ColorRgb32* row = texture.getRgb32() + (targetY * width);
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
