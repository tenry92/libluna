#include <stdio.h>
#include <string.h>

#include <libpng16/png.h>

#include <libgfx/libgfx.h>

typedef struct {
  int showHelp;
  int encode;
  int decode;
  const char *outputFile;
  const char *type;
  const char *size;
  int noPal;
  int numPals;
  const char **pals;
  const char **palFormat;
  const char **palRect;
  int palOnly;
  const char *format;
  const char *rect;
  int numTiles;
  int numAnims;
  const char **anims;
  const char *inputFile;
} Options;

static uint8_t expand5BitTo8Bit(uint8_t value) {
  return (value << 3) | (value >> 2);
}

static int parseCommaSeparatedIntegers(const char *input, int *output, int size) {
  int count = 0;
  char *token;
  char *inputCopy = strdup(input);
  count = 0;

  token = strtok(inputCopy, ",");

  while (token != NULL && count < size) {
    output[count] = atoi(token);
    ++count;
    token = strtok(NULL, ",");
  }

  free(inputCopy);

  return count;
}

static void printUsage() {
  const char *command = "gfxtool";

  printf("Usage: %s [OPTIONS] input_file\n\n", command);

  printf("Description:\n");
  printf("  %s is used to convert from and to GFX images and extract\n", command);
  printf("  information of GFX files.\n\n");
  
  printf("Options:\n");
  printf("  -h, --help              Show this help message and exit.\n");
  printf("  -e, --encode            Convert input_file to GFX.\n");
  printf("  -d, --decode            Convert input_file to PNG.\n");
  printf("  -o, --output FILE       Output file to write.\n\n");

  printf("Options (encoding):\n");
  printf("  -t, --type TYPE         Specify GFX type to generate:\n");
  printf("                          image (default), tileset.\n");
  printf("                          If not provided, it's image by default.\n");
  printf("  -s, --size w,h          Specify width and height of a single frame\n");
  printf("                          or tile.\n");
  printf("                          Frames and tiles are counted from top left,\n");
  printf("                          to the right and then to the bottom.\n");
  printf("                          You can use --rect to restrict the\n");
  printf("                          extraction to a given area.\n");
  printf("  --no-pal                For indexed input file, don't include any\n");
  printf("                          palette.\n");
  printf("  --pal PALETTE           Include palette from file PALETTE.\n");
  printf("                          If this option is given, any palette from\n");
  printf("                          input_file is ignored.\n");
  printf("                          You can use this option multiple times to\n");
  printf("                          include multiple palettes.\n\n");
  printf("                          If PALETTE is an image file (not containing\n");
  printf("                          an actual palette), you have to specify the\n");
  printf("                          number of columns per row (and optionally\n");
  printf("                          by column):\n");
  printf("                          --pal palette.png,16\n");
  printf("                          --pal palette.png,8,8\n");
  printf("  --pal-only              Include palette in output GFX only.\n");
  printf("  -f, --format FORMAT     Color format to use. Supported formats are:\n");
  printf("                          ci4     4-bit color indexed.\n");
  printf("                          ci8     8-bit color indexed.\n");
  printf("                          i4      4-bit grayscale.\n");
  printf("                          i8      8-bit grayscale.\n");
  printf("                          rgba16  5-bit red, 5-bit green, 5-bit blue,\n");
  printf("                                  1-bit alpha.\n");
  printf("                          rgb24   8-bit for red, green and blue.\n");
  printf("                          rgba32  8-bit for red, green, blue and\n");
  printf("                                  alpha. (default)\n");
  printf("  --pal-format            Color format to use for palettes. Supports\n");
  printf("                          the same values as --format, except for\n");
  printf("                          ci4 and ci8.\n");
  printf("  --pal-rect x,y,w,h      Only consider the given rect for the\n");
  printf("                          following palette image for extracting the\n");
  printf("                          palette colors.\n");
  printf("  --rect x,y,w,h          When encoding a GFX file, only consider the\n");
  printf("                          given rectangle from input_file.\n");
  printf("                          The origin is the top left corner.\n\n");
  printf("  --tilecount N           Only include the first N tiles in the\n");
  printf("                          actual tileset. Any additional frames are\n");
  printf("                          considered animation frames for the other\n");
  printf("                          tiles.\n");
  printf("                          Defaults to the total number of frames.\n");
  printf("  -a, --anim ANIM         Define an animation to include in the GFX\n");
  printf("                          file.\n\n");
  printf("                          If type is image, this is a series of frame\n");
  printf("                          indices, for example:\n");
  printf("                          -a 0,1,2,1\n\n");
  printf("                          If type is tileset, this provides the tile\n");
  printf("                          index to animate and area from input_file\n");
  printf("                          for animation frames.\n");
  printf("                          In this case, --rect is usually also\n");
  printf("                          provided to exclude the animation frames\n");
  printf("                          from the default tileset.\n");
  printf("                          The syntax for tile animation is:\n");
  printf("                          -a i,n,x,y\n");
  printf("                            i    Tile index to animate (starting at\n");
  printf("                                 0).\n");
  printf("                            n    Number of (additional) frames in\n");
  printf("                                 animation.\n");
  printf("                            x,y  Top-left coordinate of the first\n");
  printf("                                 frame.\n");
  printf("                                 The frames must be ordered from left\n");
  printf("                                 to right.\n\n");
  printf("                          You can use multiple -a options to include\n");
  printf("                          multiple animations.\n\n");

  // printf("Options (decoding):\n");
  // printf("  -l LAYOUT               Method for frames layout. LAYOUT is one of:\n");
  // printf("                          horizontal, vertical, rect (default).\n\n");

  printf("Positional Arguments:\n");
  printf("  input_file              Image file to take as input.\n\n");

  printf("Examples:\n");
  printf("  %s -e input.png -o output.gfx\n", command);
  printf("    Convert input.png to output.gfx.\n\n");
  printf("  %s -d intput.gfx -o output.png\n", command);
  printf("    Convert input.gfx to output.png.\n\n");
  printf("  %s -e input.png -s 32,32 output.gfx\n", command);
  printf("    Convert input.png with 32x32 pixels frames to output.gfx.\n\n");
  printf("  %s -e input.png -s 8,8 -t tileset output.gfx\n", command);
  printf("    Convert input.png to an 8x8 tileset to output.gfx.\n\n");
  printf("  %s -e input.png --no-pal output.png\n", command);
  printf("    Convert input.png to output.gfx but omit palette.\n\n");
  printf("  %s -e input.png --pal palette.png -o output.gfx\n", command);
  printf("    Convert input.png to output.gfx using palette.png.\n\n");
  printf("  %s -e input.png --rect 0,0,32,32 --pal-rect 0,32,32,8\n", command);
  printf("  --pal input.png -o output.gfx\n");
  printf("    Convert input.png to output.gfx using palette included within\n");
  printf("    the same image file.\n");
  printf("  %s --pal-only palette.png -o output.gfx\n", command);
  printf("    Convert palette.png to output.gfx, only containing the palette.\n\n");
  printf("  %s --pal-only --pal pal1.png --pal pal2.png -o output.gfx\n", command);
  printf("    Write given palettes to output.gfx.\n\n");
  printf("  %s -e input.png -s 8,8 -a 0,1 -a 2,3,4,3 -o output.gfx\n", command);
  printf("    Convert input.png to output.gfx, including two animations.\n\n");
  printf("  %s -e input.png -s 8,8 -t tileset --rect 0,0,16,16 -a 1,2,0,16\n", command);
  printf("  -o output.gfx\n");
  printf("    Convert input.png to output.gfx, including one animation for tile\n");
  printf("    index 1, having two additional frames at 0,16.\n");
}

static void parseOptions(int argc, char **argv, Options *options) {
  const char *nextPalFormat = NULL;
  const char *nextPalRect = NULL;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      options->showHelp = 1;
    } else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--encode") == 0) {
      options->encode = 1;
    } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--decode") == 0) {
      options->decode = 1;
    } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      options->outputFile = argv[++i];
    } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--type") == 0) {
      options->type = argv[++i];
    } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0) {
      options->size = argv[++i];
    } else if (strcmp(argv[i], "--no-pal") == 0) {
      options->noPal = 1;
    } else if (strcmp(argv[i], "--pal") == 0) {
      ++options->numPals;

      if (options->numPals == 1) {
        options->pals = malloc(options->numPals * sizeof(const char *));
        options->palFormat = malloc(options->numPals * sizeof(const char *));
        options->palRect = malloc(options->numPals * sizeof(const char *));
      } else {
        options->pals = realloc(options->pals, options->numPals * sizeof(const char *));
        options->palFormat = realloc(options->pals, options->numPals * sizeof(const char *));
        options->palRect = realloc(options->pals, options->numPals * sizeof(const char *));
      }

      options->pals[options->numPals - 1] = argv[++i];
      options->palFormat[options->numPals - 1] = nextPalFormat;
      options->palRect[options->numPals - 1] = nextPalRect;
    } else if (strcmp(argv[i], "--pal-only") == 0) {
      options->palOnly = 1;
    } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0) {
      options->format = argv[++i];
    } else if (strcmp(argv[i], "--pal-format") == 0) {
      nextPalFormat = argv[++i];
    } else if (strcmp(argv[i], "--pal-rect") == 0) {
      nextPalRect = argv[++i];
    } else if (strcmp(argv[i], "--rect") == 0) {
      options->rect = argv[++i];
    } else if (strcmp(argv[i], "--tilecount") == 0) {
      options->numTiles = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--anim") == 0) {
      ++options->numAnims;

      if (options->numAnims == 1) {
        options->anims = malloc(options->numAnims * sizeof(const char *));
      } else {
        options->anims = realloc(options->anims, options->numAnims * sizeof(const char *));
      }

      options->anims[options->numAnims - 1] = argv[++i];
    } else if (argv[i][0] != '-') {
      if (options->inputFile == NULL) {
        options->inputFile = argv[i];
      }
    } else {
      printf("Unknown option %s.\n", argv[i]);
    }
  }
}

static int encodePaletteFromFile(libgfx_Image *image, Options *options, int palIndex) {
  char filename[256] = {0};
  int colorsPerRow = 1;
  int colorsPerCol = 1;
  
  const char *palName = options->pals[palIndex];
  const char *palRect = options->palRect[palIndex];

  char temp[256];
  strncpy(temp, palName, sizeof(temp) - 1);

  char *token = strtok(temp, ",");

  if (token != NULL) {
    strncpy(filename, token, sizeof(filename) - 1);
    filename[sizeof(filename) - 1] = 0;
  }

  token = strtok(NULL, ",");

  if (token != NULL) {
    colorsPerRow = atoi(token);
  }

  token = strtok(NULL, ",");

  if (token != NULL) {
    colorsPerCol = atoi(token);
  }

  FILE *fp = fopen(filename, "rb");

  if (!fp) {
    fprintf(stderr, "Error opening %s for reading.\n", filename);
    return 1;
  }

  printf("Reading %s\n", filename);

  png_structp png = png_create_read_struct(
    PNG_LIBPNG_VER_STRING, NULL, NULL, NULL
  );

  if (!png) {
    fclose(fp);
    fprintf(stderr, "Error creating png struct.\n");
    return 1;
  }

  png_infop info = png_create_info_struct(png);

  if (!info) {
    png_destroy_read_struct(&png, NULL, NULL);
    fclose(fp);
    fprintf(stderr, "Error creating png info struct.\n");
    return 1;
  }

  png_init_io(png, fp);
  png_read_info(png, info);

  int inputWidth = png_get_image_width(png, info);
  int inputHeight = png_get_image_height(png, info);
  png_byte inputColorType = png_get_color_type(png, info);
  png_byte inputBitDepth = png_get_bit_depth(png, info);

  if (inputBitDepth == 16) {
    png_set_strip_16(png);
    inputBitDepth = 8;
  }

  if (inputColorType == PNG_COLOR_TYPE_GRAY) {
    png_set_expand_gray_1_2_4_to_8(png);
  }

  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
  }

  if (inputColorType == PNG_COLOR_TYPE_GRAY || inputColorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png);
  }

  if (inputColorType == PNG_COLOR_TYPE_RGB || inputColorType == PNG_COLOR_TYPE_GRAY || inputColorType == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
  }

  int rectLeft = 0;
  int rectTop = 0;
  int rectWidth = inputWidth;
  int rectHeight = inputHeight;

  if (palRect) {
    int rect[4];
    int count = parseCommaSeparatedIntegers(palRect, rect, 4);

    if (count != 4) {
      printf("Please provide x, y, width and height for pal rect.\n");
      return 1;
    }

    rectLeft = rect[0];
    rectTop = rect[1];
    rectWidth = rect[2];
    rectHeight = rect[3];
  }

  int numColors = colorsPerRow * colorsPerCol;
  int colorWidth = rectWidth / colorsPerRow;
  int colorHeight = rectHeight / colorsPerCol;

  png_read_update_info(png, info);

  png_bytep *inputRowPointers = (png_bytep *) malloc(inputHeight * sizeof(png_bytep));
  for (int row = 0; row < inputHeight; ++row) {
    inputRowPointers[row] = (png_byte *) malloc(png_get_rowbytes(png, info));
  }
  png_read_image(png, inputRowPointers);

  libgfx_ColorFormat palFormat = LIBGFX_FORMAT_32BIT_RGBA;

  if (options->palFormat[palIndex]) {
    if (strcmp(options->palFormat[palIndex], "i4") == 0) {
      palFormat = LIBGFX_FORMAT_4BIT_GRAYSCALE;
    } else if (strcmp(options->palFormat[palIndex], "i8") == 0) {
      palFormat = LIBGFX_FORMAT_8BIT_GRAYSCALE;
    } else if (strcmp(options->palFormat[palIndex], "rgba16") == 0) {
      palFormat = LIBGFX_FORMAT_16BIT_RGBA;
    } else if (strcmp(options->palFormat[palIndex], "rgb24") == 0) {
      palFormat = LIBGFX_FORMAT_24BIT_RGB;
    } else if (strcmp(options->palFormat[palIndex], "rgba32") == 0) {
      palFormat = LIBGFX_FORMAT_32BIT_RGBA;
    }
  }

  libgfx_Palette *pal = libgfx_getPalette(image, palIndex);
  libgfx_allocPalette(image, pal, palFormat, numColors);

  for (int i = 0; i < numColors; ++i) {
    int x = (i % colorsPerRow) * colorWidth + colorWidth / 2 + rectLeft;
    int y = (i / colorsPerRow) * colorHeight + colorHeight / 2 + rectTop;
    png_bytep pointer = inputRowPointers[y] + (x * 4);
    uint8_t red = pointer[0];
    uint8_t green = pointer[1];
    uint8_t blue = pointer[2];
    uint8_t alpha = pointer[3];

    switch (pal->colorFormat) {
      case LIBGFX_FORMAT_4BIT_GRAYSCALE:
        break;
      case LIBGFX_FORMAT_8BIT_GRAYSCALE:
        ((uint8_t *) pal->colors)[i] = (red + green + blue) / 3;
        break;
      case LIBGFX_FORMAT_16BIT_RGBA: {
        libgfx_16BitPixel *rgbaPointer = libgfx_to16BitPixels(pal->colors);
        rgbaPointer[i].red = red >> 3;
        rgbaPointer[i].green = green >> 3;
        rgbaPointer[i].blue = blue >> 3;
        rgbaPointer[i].alpha = alpha >> 7;
        break;
      }
      case LIBGFX_FORMAT_24BIT_RGB:
        ((uint8_t *) pal->colors)[i * 3] = red;
        ((uint8_t *) pal->colors)[i * 3 + 1] = green;
        ((uint8_t *) pal->colors)[i * 3 + 2] = blue;
        break;
      case LIBGFX_FORMAT_32BIT_RGBA:
        ((uint8_t *) pal->colors)[i * 4] = red;
        ((uint8_t *) pal->colors)[i * 4 + 1] = green;
        ((uint8_t *) pal->colors)[i * 4 + 2] = blue;
        ((uint8_t *) pal->colors)[i * 4 + 3] = alpha;
        break;
    }
  }

  fclose(fp);
}

static int findColorIndex(libgfx_Palette *palette, int red, int green, int blue, int alpha) {
  int selectedIndex = 0;
  int leastDiff = 0x1000000;

  for (int colIndex = 0; colIndex < palette->numColors; ++colIndex) {
    int colorRed, colorGreen, colorBlue, colorAlpha;

    switch (palette->colorFormat) {
      case LIBGFX_FORMAT_16BIT_RGBA:
        colorRed = expand5BitTo8Bit(libgfx_to16BitPixels(palette->colors)[colIndex].red);
        colorGreen = expand5BitTo8Bit(libgfx_to16BitPixels(palette->colors)[colIndex].green);
        colorBlue = expand5BitTo8Bit(libgfx_to16BitPixels(palette->colors)[colIndex].blue);
        colorAlpha = libgfx_to16BitPixels(palette->colors)[colIndex].alpha ? 255 : 0;
        break;
      case LIBGFX_FORMAT_32BIT_RGBA:
        colorRed = ((uint8_t *) palette->colors)[colIndex * 4];
        colorGreen = ((uint8_t *) palette->colors)[colIndex * 4 + 1];
        colorBlue = ((uint8_t *) palette->colors)[colIndex * 4 + 2];
        colorAlpha = ((uint8_t *) palette->colors)[colIndex * 4 + 3];
        break;
    }

    int diff = abs(colorRed - red) + abs(colorGreen - green) + abs(colorBlue - blue) + abs(colorAlpha - alpha);

    if (diff < leastDiff) {
      selectedIndex = colIndex;
      leastDiff = diff;
    }

    if (diff == 0) {
      break;
    }
  }

  return selectedIndex;
}

static int encode(Options *options) {
  if (options->inputFile == NULL) {
    printf("No input file provided.\n");
    return 1;
  }

  FILE *fp = fopen(options->inputFile, "rb");

  if (!fp) {
    fprintf(stderr, "Error opening %s for reading.\n", options->inputFile);
    return 1;
  }

  printf("Reading %s\n", options->inputFile);

  libgfx_ImageType type = LIBGFX_TYPE_IMAGE;

  if (options->type) {
    if (strcmp(options->type, "tileset") == 0) {
      type = LIBGFX_TYPE_TILESET;
    } else if (strcmp(options->type, "image") != 0) {
      printf("Unknown type %s.\n", options->type);
      return 1;
    }
  }

  libgfx_ColorFormat format = LIBGFX_FORMAT_32BIT_RGBA;

  if (options->format) {
    if (strcmp(options->format, "ci4") == 0) {
      format = LIBGFX_FORMAT_4BIT_INDEXED;
    } else if (strcmp(options->format, "ci8") == 0) {
      format = LIBGFX_FORMAT_8BIT_INDEXED;
    } else if (strcmp(options->format, "i4") == 0) {
      format = LIBGFX_FORMAT_4BIT_GRAYSCALE;
    } else if (strcmp(options->format, "i8") == 0) {
      format = LIBGFX_FORMAT_8BIT_GRAYSCALE;
    } else if (strcmp(options->format, "rgba16") == 0) {
      format = LIBGFX_FORMAT_16BIT_RGBA;
    } else if (strcmp(options->format, "rgb24") == 0) {
      format = LIBGFX_FORMAT_24BIT_RGB;
    } else if (strcmp(options->format, "rgba32") != 0) {
      printf("Unknown format %s.\n", options->format);
      return 1;
    }
  }

  png_structp png = png_create_read_struct(
    PNG_LIBPNG_VER_STRING, NULL, NULL, NULL
  );

  if (!png) {
    fclose(fp);
    fprintf(stderr, "Error creating png struct.\n");
    return 1;
  }

  png_infop info = png_create_info_struct(png);

  if (!info) {
    png_destroy_read_struct(&png, NULL, NULL);
    fclose(fp);
    fprintf(stderr, "Error creating png info struct.\n");
    return 1;
  }

  png_init_io(png, fp);
  png_read_info(png, info);

  int inputWidth = png_get_image_width(png, info);
  int inputHeight = png_get_image_height(png, info);
  png_byte inputColorType = png_get_color_type(png, info);
  png_byte inputBitDepth = png_get_bit_depth(png, info);

  if (inputBitDepth == 16) {
    png_set_strip_16(png);
    inputBitDepth = 8;
  }

  if (inputColorType == PNG_COLOR_TYPE_GRAY) {
    png_set_expand_gray_1_2_4_to_8(png);
  }

  int frameWidth = inputWidth;
  int frameHeight = inputHeight;

  if (options->size) {
    int size[2];
    int count = parseCommaSeparatedIntegers(options->size, size, 2);

    if (count != 2) {
      printf("Please provide width and height for size.\n");
      return 1;
    }

    frameWidth = size[0];
    frameHeight = size[1];
  }

  int rectLeft = 0;
  int rectTop = 0;
  int rectWidth = inputWidth;
  int rectHeight = inputHeight;

  if (options->rect) {
    int rect[4];
    int count = parseCommaSeparatedIntegers(options->rect, rect, 4);

    if (count != 4) {
      printf("Please provide x, y, width and height for rect.\n");
      return 1;
    }

    rectLeft = rect[0];
    rectTop = rect[1];
    rectWidth = rect[2];
    rectHeight = rect[3];
  }

  int framesPerRow = rectWidth / frameWidth;
  int framesPerCol = rectHeight / frameHeight;
  int numFrames = framesPerCol * framesPerRow;

  png_read_update_info(png, info);

  libgfx_Image *image = libgfx_createImage(frameWidth, frameHeight, format, type);

  if (options->numPals) {
    libgfx_allocPalettes(image, options->numPals);

    for (int i = 0; i < options->numPals; ++i) {
      int result = encodePaletteFromFile(image, options, i);

      if (result) {
        return result;
      }
    }
  }

  printf("Allocating %d frames\n", numFrames);
  libgfx_allocFrames(image, numFrames);

  png_bytep *inputRowPointers = (png_bytep *) malloc(inputHeight * sizeof(png_bytep));
  for (int row = 0; row < inputHeight; ++row) {
    inputRowPointers[row] = (png_byte *) malloc(png_get_rowbytes(png, info));
  }
  png_read_image(png, inputRowPointers);

  for (int frameRow = 0; frameRow < framesPerCol; ++frameRow) {
    for (int frameCol = 0; frameCol < framesPerRow; ++frameCol) {
      int frameIndex = frameCol + frameRow * framesPerRow;
      printf("Writing frame #%d\n", frameIndex);
      uint8_t *frame = (uint8_t *) libgfx_getFramePointer(image, frameIndex);

      for (int row = 0; row < frameHeight; ++row) {
        png_bytep inputRow = inputRowPointers[rectTop + frameRow * frameHeight + row];

        for (int col = 0; col < frameWidth; ++col) {
          uint8_t red, green, blue, alpha, gray, colorIndex;
          int isRgb = 0;
          int hasAlpha = 0;
          int isGray = 0;
          int isIndexed = 0;
          int isOdd = col % 2;

          switch (inputColorType) {
            default:
              fprintf(stderr, "unsupported input color type.\n");
              return 1;
            case PNG_COLOR_TYPE_RGBA:
              isRgb = 1;
              hasAlpha = 1;
              red = inputRow[(col + frameCol * frameWidth) * 4];
              green = inputRow[(col + frameCol * frameWidth) * 4 + 1];
              blue = inputRow[(col + frameCol * frameWidth) * 4 + 2];
              alpha = inputRow[(col + frameCol * frameWidth) * 4 + 3];
              break;
            case PNG_COLOR_TYPE_RGB:
              isRgb = 1;
              red = inputRow[(col + frameCol * frameWidth) * 3];
              green = inputRow[(col + frameCol * frameWidth) * 3 + 1];
              blue = inputRow[(col + frameCol * frameWidth) * 3 + 2];
              break;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
              isGray = 1;
              hasAlpha = 1;
              gray = inputRow[(col + frameCol * frameWidth) * 2];
              alpha = inputRow[(col + frameCol * frameWidth) * 2 + 1];
              break;
            case PNG_COLOR_TYPE_GRAY:
              isGray = 1;
              gray = inputRow[(col + frameCol * frameWidth)];
              break;
            case PNG_COLOR_TYPE_PALETTE: {
              int valuesPerByte = 8 / inputBitDepth;
              int byteIndex = (col + frameCol * frameWidth) / valuesPerByte;
              int bitOffset = (col % valuesPerByte) * inputBitDepth;
              int mask = (1 << inputBitDepth) - 1;
              uint8_t byte = inputRow[byteIndex];
              colorIndex = byte >> (8 - inputBitDepth - bitOffset) & mask;
              break;
            }
          }

          if (!hasAlpha) {
            alpha = 255;
          }

          if (isGray) {
            red = green = blue = gray;
          } else {
            gray = (red + green + blue) / 3;
          }
          
          switch (format) {
            case LIBGFX_FORMAT_4BIT_INDEXED:
              if (!isIndexed) {
                colorIndex = findColorIndex(libgfx_getPalette(image, 0), red, green, blue, alpha);
              }

              if (isOdd) {
                frame[(col + row * frameHeight) / 2] |= colorIndex << 4;
              } else {
                frame[(col + row * frameHeight) / 2] |= colorIndex;
              }
              break;
            case LIBGFX_FORMAT_8BIT_INDEXED:
              if (!isIndexed) {
                colorIndex = findColorIndex(libgfx_getPalette(image, 0), red, green, blue, alpha);
              }

              frame[col + row * frameHeight] = colorIndex;
              break;
            case LIBGFX_FORMAT_4BIT_GRAYSCALE:
              if (isOdd) {
                frame[(col + row * frameHeight) / 2] |= gray << 4;
              } else {
                frame[(col + row * frameHeight) / 2] |= gray;
              }
              break;
            case LIBGFX_FORMAT_8BIT_GRAYSCALE:
              frame[col + row * frameHeight] = gray;
              break;
            case LIBGFX_FORMAT_16BIT_RGBA: {
              libgfx_16BitPixel *ref = (libgfx_16BitPixel *) (frame + (col + row * frameHeight) * 2);
              ref->red = red >> 3;
              ref->green = green >> 3;
              ref->blue = blue >> 3;
              ref->alpha = alpha >> 7;
              break;
            }
            case LIBGFX_FORMAT_24BIT_RGB:
              frame[(col + row * frameHeight) * 3] = red;
              frame[(col + row * frameHeight) * 3 + 1] = green;
              frame[(col + row * frameHeight) * 3 + 2] = blue;
              break;
            case LIBGFX_FORMAT_32BIT_RGBA:
              frame[(col + row * frameHeight) * 4] = red;
              frame[(col + row * frameHeight) * 4 + 1] = green;
              frame[(col + row * frameHeight) * 4 + 2] = blue;
              frame[(col + row * frameHeight) * 4 + 3] = alpha;
              break;
          }
        }
      }
    }
  }

  fclose(fp);

  if (options->outputFile != NULL) {
    printf("Writing %s\n", options->outputFile);
    if (libgfx_writeImageToFile(image, options->outputFile)) {
      fprintf(stderr, "Error writing %s\n", options->outputFile);
      return 1;
    }
  }

  return 0;
}

static int decode(Options *options) {
  if (options->inputFile == NULL) {
    printf("No input file provided.\n");
    return 1;
  }

  printf("Reading %s\n", options->inputFile);

  libgfx_Image *image = libgfx_loadImageFromFile(options->inputFile);

  if (!image) {
    fprintf(stderr, "input is not a valid GFX file: %s\n", libgfx_getError());
    return 1;
  }

  printf("width: %d\n", image->width);
  printf("height: %d\n", image->height);
  printf("format: %d\n", image->colorFormat);
  printf("type: %d\n", image->type);
  printf("frames: %d\n", image->numFrames);
  printf("tiles: %d\n", image->numTiles);
  printf("anims: %d\n", image->numAnimations);
  printf("pals: %d\n", image->numTiles);

  int outputWidth = image->width * image->numFrames;
  int outputHeight = image->height;

  png_bytep *outputRowPointers = (png_bytep *) malloc(outputHeight * sizeof(png_bytep));
  for (int row = 0; row < outputHeight; ++row) {
    outputRowPointers[row] = (png_byte *) malloc(4 * outputWidth * sizeof(png_byte));
  }

  for (int frameIndex = 0; frameIndex < image->numFrames; ++frameIndex) {
    printf("Decoding frame #%d\n", frameIndex);
    uint8_t *frame = (uint8_t *) libgfx_getFramePointer(image, frameIndex);

    for (int y = 0; y < image->height; ++y) {
      for (int x = 0; x < image->width; ++x) {
        switch (image->colorFormat) {
          case LIBGFX_FORMAT_4BIT_INDEXED: {
            libgfx_4BitPixel *pixels = libgfx_to4BitPixels(frame);
            libgfx_4BitPixel pixel = pixels[(x + y * image->width) / 2];
            int isOdd = x % 2;
            int colorIndex;

            if (isOdd) {
              colorIndex = pixel.second;
            } else {
              colorIndex = pixel.first;
            }

            // todo: choose color from palette, if available

            outputRowPointers[y][(frameIndex * image->width + x) * 4] = (colorIndex << 4) | colorIndex;
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 1] = (colorIndex << 4) | colorIndex;
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 2] = (colorIndex << 4) | colorIndex;
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 3] = 255;

            break;
          }
          case LIBGFX_FORMAT_8BIT_INDEXED:
            outputRowPointers[y][(frameIndex * image->width + x) * 4] = frame[x + y * image->width];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 1] = frame[x + y * image->width];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 2] = frame[x + y * image->width];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 3] = 255;
            break;
          case LIBGFX_FORMAT_4BIT_GRAYSCALE:
            fprintf(stderr, "Decoding 4-bit GFX file is not implemented.\n");
            return 1;
          case LIBGFX_FORMAT_8BIT_GRAYSCALE:
            outputRowPointers[y][(frameIndex * image->width + x) * 4] = frame[x + y * image->width];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 1] = frame[x + y * image->width];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 2] = frame[x + y * image->width];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 3] = 255;
            break;
          case LIBGFX_FORMAT_16BIT_RGBA: {
            libgfx_16BitPixel *rgbaFrame = libgfx_to16BitPixels(frame);
            libgfx_16BitPixel value = rgbaFrame[x + y * image->width];
            outputRowPointers[y][(frameIndex * image->width + x) * 4] = expand5BitTo8Bit(value.red);
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 1] = expand5BitTo8Bit(value.green);
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 2] = expand5BitTo8Bit(value.blue);
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 3] = value.alpha ? 255 : 0;
            break;
          }
          case LIBGFX_FORMAT_24BIT_RGB:
            outputRowPointers[y][(frameIndex * image->width + x) * 4] = frame[(x + y * image->width) * 3];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 1] = frame[(x + y * image->width) * 3 + 1];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 2] = frame[(x + y * image->width) * 3 + 2];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 3] = 255;
            break;
          case LIBGFX_FORMAT_32BIT_RGBA:
            outputRowPointers[y][(frameIndex * image->width + x) * 4] = frame[(x + y * image->width) * 4];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 1] = frame[(x + y * image->width) * 4 + 1];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 2] = frame[(x + y * image->width) * 4 + 2];
            outputRowPointers[y][(frameIndex * image->width + x) * 4 + 3] = frame[(x + y * image->width) * 4 + 3];
            break;
        }
      }
    }
  }

  FILE *fp = fopen(options->outputFile, "wb");

  if (!fp) {
    fprintf(stderr, "Error opening %s\n", options->outputFile);
    return 1;
  }

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info = png_create_info_struct(png);
  png_init_io(png, fp);
  png_set_IHDR(
    png, info, outputWidth, outputHeight,
    8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);
  png_write_image(png, outputRowPointers);
  png_write_end(png, NULL);
  fclose(fp);

  png_destroy_write_struct(&png, &info);

  return 0;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    printUsage();
    return 0;
  }

  Options options;
  memset(&options, 0, sizeof(options));
  parseOptions(argc, argv, &options);

  if (options.showHelp) {
    printUsage();
    return 0;
  }

  if (options.encode) {
    return encode(&options);
  } else if (options.decode) {
    return decode(&options);
  } else {
    printf("Please provide either -e, --encode, -d or --decode.\n\n");
    printUsage();
    return 1;
  }
}
