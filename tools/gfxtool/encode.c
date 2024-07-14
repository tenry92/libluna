#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "encode.h"
#include "surface.h"
#include "utils.h"
#include "loadpng.h"
#include "bmfont.h"

#include <libgfx/libgfx.h>

typedef struct {
  int rectX;
  int rectY;
  int rectW;
  int rectH;
  int useRect;
  int frameWidth;
  int frameHeight;
  libgfx_ColorFormat colorFormat;
  libgfx_Gfx *gfx;
  int nextPaletteIndex;
  int nextFramesetIndex;
  int nextAnimationIndex;
} EncodingState;

static int parseStringCommaIntegers(const char *input, char *string, int *output, int size) {
  char *buffer = strdup(input);
  int count = 0;
  char *token = strtok(buffer, ",");

  if (token != NULL) {
    strcpy(string, token);
    ++count;

    token = strtok(NULL, ",");

    while (token != NULL && count < size) {
      output[count - 1] = atoi(token);
      ++count;
      token = strtok(NULL, ",");
    }
  }

  free(buffer);

  return count;
}

static void replaceFilename(char *path, const char *newFilename) {
  char *lastSlash = strrchr(path, '/');

  if (lastSlash != NULL) {
    *(lastSlash + 1) = 0;
    strcat(path, newFilename);
  } else {
    strcpy(path, newFilename);
  }
}

static int setRect(EncodingState *state, const char *arg) {
  int numbers[4];
  int count = parseCommaSeparatedIntegers(arg, numbers, 4);

  if (count < 4) {
    fprintf(stderr, "--rect expects 4 numbers\n");
    return 1;
  }

  state->rectX = numbers[0];
  state->rectY = numbers[1];
  state->rectW = numbers[2];
  state->rectH = numbers[3];
  state->useRect = 1;

  return 0;
}

static int setSize(EncodingState *state, const char *arg) {
  int numbers[2];
  int count = parseCommaSeparatedIntegers(arg, numbers, 2);

  if (count < 2) {
    fprintf(stderr, "--rect expects 2 numbers\n");
    return 1;
  }

  state->frameWidth = numbers[0];
  state->frameHeight = numbers[1];

  return 0;
}

static int setFormat(EncodingState *state, const char *arg) {
  if (strcmp("ci4", arg) == 0) {
    state->colorFormat = LIBGFX_COLOR_4BIT_INDEXED;
  } else if (strcmp("ci8", arg) == 0) {
    state->colorFormat = LIBGFX_COLOR_8BIT_INDEXED;
  } else if (strcmp("i4", arg) == 0) {
    state->colorFormat = LIBGFX_COLOR_4BIT_GRAYSCALE;
  } else if (strcmp("i8", arg) == 0) {
    state->colorFormat = LIBGFX_COLOR_8BIT_GRAYSCALE;
  } else if (strcmp("rgba16", arg) == 0) {
    state->colorFormat = LIBGFX_COLOR_16BIT_RGBA;
  } else if (strcmp("rgb24", arg) == 0) {
    state->colorFormat = LIBGFX_COLOR_24BIT_RGB;
  } else if (strcmp("rgba32", arg) == 0) {
    state->colorFormat = LIBGFX_COLOR_32BIT_RGBA;
  } else {
    fprintf(stderr, "Unknown format %s\n", arg);
    return 1;
  }

  return 0;
}

static int addPalette(EncodingState *state, const char *arg) {
  char filename[256];
  int count[2];
  count[0] = 1;
  count[1] = 1;
  int numValues = parseStringCommaIntegers(arg, filename, count, 2);

  if (numValues < 2) {
    fprintf(stderr, "--pal requires filename and color count\n");
    return 1;
  }

  if (count[0] < 1 || count[1] < 1) {
    fprintf(stderr, "--pal requires at least 1 color\n");
    return 1;
  }

  Surface *surface = loadPng(filename);

  if (!surface) {
    return 1;
  }

  if (state->useRect) {
    Surface *cropped = cropSurface(surface, state->rectX, state->rectY, state->rectW, state->rectH);
    freeSurface(surface);
    surface = cropped;
  }

  int numColors = count[0] * count[1];
  int widthPerColor = surface->width / count[0];
  int heightPerColor = surface->height / count[1];

  printf("Adding palette %s with %d colors\n", filename, numColors);

  libgfx_Palette *pal = &state->gfx->palettes[state->nextPaletteIndex++];
  pal->colorFormat = state->colorFormat;
  libgfx_allocPaletteColors(pal, numColors);

  for (int y = 0; y < count[1]; ++y) {
    for (int x = 0; x < count[0]; ++x) {
      int offset = x + y * count[0];
      int px = x * widthPerColor + widthPerColor / 2;
      int py = y * heightPerColor + heightPerColor / 2;
      ColorRgba32 *color = getSurfacePixel(surface, px, py);

      printf("  Color #%02X: %d, %d, %d, %d (sampled at %d,%d)\n",
        offset, color->red, color->green, color->blue, color->alpha,
        state->rectX + px, state->rectY + py);

      switch (pal->colorFormat) {
        case LIBGFX_COLOR_16BIT_RGBA:
          setColor16BitRgba(pal->colors, offset, color->red >> 3, color->green >> 3, color->blue >> 3, color->alpha >> 7);
          break;
        case LIBGFX_COLOR_24BIT_RGB:
          setColor24BitRgb(pal->colors, offset, color->red, color->green, color->blue);
          break;
        case LIBGFX_COLOR_32BIT_RGBA:
          setColor32BitRgba(pal->colors, offset, color->red, color->green, color->blue, color->alpha);
          break;
      }
    }
  }

  freeSurface(surface);

  return 0;
}

static int addImage(EncodingState *state, const char *arg) {
  char filename[256];
  int frameSize[2];
  frameSize[0] = state->frameWidth;
  frameSize[1] = state->frameHeight;
  int numValues = parseStringCommaIntegers(arg, filename, frameSize, 2);

  Surface *surface = loadPng(filename);

  if (!surface) {
    return 1;
  }

  if (state->useRect) {
    Surface *cropped = cropSurface(surface, state->rectX, state->rectY, state->rectW, state->rectH);
    freeSurface(surface);
    surface = cropped;
  }

  if (frameSize[0] <= 0) {
    frameSize[0] = surface->width;
  }

  if (frameSize[1] <= 0) {
    frameSize[1] = surface->height;
  }

  int xFrameCount = surface->width / frameSize[0];
  int yFrameCount = surface->height / frameSize[1];
  int pixelsPerFrame = frameSize[0] * frameSize[1];
  int numFrames = xFrameCount * yFrameCount;

  printf("Adding frameset\n");
  printf("  Frame count: %d (%dx%d / frame)\n", numFrames, frameSize[0], frameSize[1]);

  libgfx_Frameset *frameset = &state->gfx->framesets[state->nextFramesetIndex++];
  frameset->colorFormat = state->colorFormat;
  frameset->width = frameSize[0];
  frameset->height = frameSize[1];

  libgfx_allocFrames(frameset, numFrames);

  for (int frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
    printf("  Encoding frame #%d\n", frameIndex);
    for (int py = 0; py < frameSize[1]; ++py) {
      for (int px = 0; px < frameSize[0]; ++px) {
        int xFrame = frameIndex % xFrameCount;
        int yFrame = frameIndex / xFrameCount;

        int ix = xFrame * frameSize[0] + px;
        int iy = yFrame * frameSize[1] + py;

        ColorRgba32 *color = getSurfacePixel(surface, ix, iy);
        int outOffset = frameIndex * pixelsPerFrame + px + py * frameSize[0];

        switch (frameset->colorFormat) {
          case LIBGFX_COLOR_4BIT_INDEXED:
          case LIBGFX_COLOR_8BIT_INDEXED:
          case LIBGFX_COLOR_4BIT_GRAYSCALE:
          case LIBGFX_COLOR_8BIT_GRAYSCALE:
            fprintf(stderr, "Color format not implemented\n");
            return 1;
          case LIBGFX_COLOR_16BIT_RGBA:
            setColor16BitRgba(frameset->data, outOffset, color->red >> 3, color->green >> 3, color->blue >> 3, color->alpha >> 7);
            break;
          case LIBGFX_COLOR_24BIT_RGB:
            setColor24BitRgb(frameset->data, outOffset, color->red, color->green, color->blue);
            break;
          case LIBGFX_COLOR_32BIT_RGBA:
            setColor32BitRgba(frameset->data, outOffset, color->red, color->green, color->blue, color->alpha);
            break;
        }
      }
    }
  }

  freeSurface(surface);

  return 0;
}

static int addFont(EncodingState *state, const char *arg) {
  FILE *fp = fopen(arg, "rb");

  if (!fp) {
    fprintf(stderr, "Error opening %s\n", arg);
    return 1;
  }

  fseek(fp, 0, SEEK_END);

  long fileSize = ftell(fp);

  rewind(fp);

  char *content = (char *) malloc(fileSize + 1);

  size_t bytesRead = fread(content, 1, fileSize, fp);

  if (bytesRead != fileSize) {
    fprintf(stderr, "Error reading file\n");
    free(content);
    fclose(fp);
    return 1;
  }

  content[fileSize] = 0;

  fclose(fp);

  printf("Parsing %s\n", arg);

  BMFont *font = parseBMFont(content);

  free(content);

  printf("Reading %s as texture\n", font->pages[0].file);

  char *filename = strdup(arg);
  replaceFilename(filename, font->pages[0].file);

  Surface *surface = loadPng(filename);
  free(filename);

  if (!surface) {
    return 1;
  }

  libgfx_allocFramesets(state->gfx, font->charCount);
  libgfx_allocCharacters(state->gfx, font->charCount);

  // set locale to support UTF-8
  setlocale(LC_ALL, "");

  for (int i = 0; i < font->charCount; ++i) {
    BMFontChar *ch = &font->chars[i];
    wchar_t wstr[2];
    wstr[0] = (wchar_t) ch->id;
    wstr[1] = 0;
    char mbs[MB_CUR_MAX];
    wcstombs(mbs, wstr, sizeof(mbs));
    printf("Encoding char U+%04X (%s)\n", ch->id, mbs);

    Surface *cropped = cropSurface(surface,
      ch->x, ch->y, ch->width, ch->height);

    libgfx_Frameset *frameset = &state->gfx->framesets[i];
    frameset->colorFormat = state->colorFormat;
    frameset->width = ch->width;
    frameset->height = ch->height;
    frameset->numFrames = 1;
    libgfx_allocFrames(frameset, 1);

    for (int py = 0; py < ch->height; ++py) {
      for (int px = 0; px < ch->width; ++px) {
        ColorRgba32 *color = getSurfacePixel(cropped, px, py);
        int offset = px + py * ch->width;

        switch (frameset->colorFormat) {
          case LIBGFX_COLOR_4BIT_INDEXED:
          case LIBGFX_COLOR_8BIT_INDEXED:
          case LIBGFX_COLOR_4BIT_GRAYSCALE:
          case LIBGFX_COLOR_8BIT_GRAYSCALE:
            fprintf(stderr, "Color format not implemented\n");
            return 1;
          case LIBGFX_COLOR_16BIT_RGBA:
            setColor16BitRgba(frameset->data, offset, color->red >> 3, color->green >> 3, color->blue >> 3, color->alpha >> 7);
            break;
          case LIBGFX_COLOR_24BIT_RGB:
            setColor24BitRgb(frameset->data, offset, color->red, color->green, color->blue);
            break;
          case LIBGFX_COLOR_32BIT_RGBA:
            setColor32BitRgba(frameset->data, offset, color->red, color->green, color->blue, color->alpha);
            break;
        }
      }
    }

    freeSurface(cropped);

    libgfx_Character *gfxCh = &state->gfx->characters[i];
    gfxCh->codePoint = ch->id;
    gfxCh->frameIndex = i;
    gfxCh->xOffset = ch->xoffset;
    gfxCh->yOffset = ch->yoffset;
    gfxCh->advance = ch->xadvance;
  }

  return 0;
}

int encode(int argc, char **argv) {
  EncodingState state;
  state.useRect = 0;
  state.frameWidth = 0;
  state.frameHeight = 0;
  state.colorFormat = LIBGFX_COLOR_32BIT_RGBA;
  state.gfx = libgfx_allocGfx();
  state.nextPaletteIndex = 0;
  state.nextFramesetIndex = 0;
  state.nextAnimationIndex = 0;

  int numPalettes = 0;
  int numFramesets = 0;
  int numAnimations = 0;

  for (int argIndex = 0; argIndex < argc; ++argIndex) {
    const char *arg = argv[argIndex];

    if (isArg(arg, "-p", "--pal")) {
      ++numPalettes;
      continue;
    }

    if (isArg(arg, "-i", "--image")) {
      ++numFramesets;
      continue;
    }

    if (isArg(arg, "-a", "--anim")) {
      ++numAnimations;
      continue;
    }
  }

  if (numPalettes > 0) {
    libgfx_allocPalettes(state.gfx, numPalettes);
  }

  if (numFramesets > 0) {
    libgfx_allocFramesets(state.gfx, numFramesets);
  }

  if (numAnimations > 0) {
    libgfx_allocAnimations(state.gfx, numAnimations);
  }

  for (int argIndex = 0; argIndex < argc; ++argIndex) {
    const char *arg = argv[argIndex];
    if (isArg(arg, "-r", "--rect")) {
      if (setRect(&state, argv[++argIndex])) {
        return 1;
      }

      continue;
    }

    if (isArg(arg, "-f", "--format")) {
      if (setFormat(&state, argv[++argIndex])) {
        return 1;
      }

      continue;
    }

    if (isArg(arg, "-s", "--size")) {
      if (setSize(&state, argv[++argIndex])) {
        return 1;
      }

      continue;
    }

    if (isArg(arg, "-p", "--pal")) {
      if (addPalette(&state, argv[++argIndex])) {
        return 1;
      }

      continue;
    }

    if (isArg(arg, "-i", "--image")) {
      if (addImage(&state, argv[++argIndex])) {
        return 1;
      }

      continue;
    }

    if (isArg(arg, NULL, "--font")) {
      if (addFont(&state, argv[++argIndex])) {
        return 1;
      }

      continue;
    }

    if (isArg(arg, "-o", "--output")) {
      const char *filename = argv[++argIndex];

      printf("Writing output to %s\n", filename);

      if (libgfx_writeImageToFile(state.gfx, filename)) {
        fprintf(stderr, "Error writing GFX file: %s\n", libgfx_getError());
        return 1;
      }

      continue;
    }

    fprintf(stderr, "Unknown argument %s\n", arg);
    return 1;
  }

  libgfx_freeGfx(state.gfx);

  return 0;
}
