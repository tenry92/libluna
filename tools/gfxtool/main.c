#include <stdio.h>
#include <string.h>

#include <libpng16/png.h>

#include <libgfx/libgfx.h>

#include "surface.h"
#include "loadpng.h"
#include "encode.h"

static void printUsage() {
  const char *command = "gfxtool";

  printf("Usage: %s [command] [OPTIONS]\n\n", command);

  printf("Description:\n");
  printf("  %s is used to convert from and to GFX files and extract\n", command);
  printf("  information of GFX files.\n\n");

  printf("Commands:\n");
  printf("  encode                  Create a GFX file\n");
  printf("  decode                  Decode GFX file\n\n");

  printf("Encoding:\n");
  printf("When encoding a GFX file, one or multiple image and additional files\n");
  printf("will be converted into a single GFX file.\n");
  printf("Each input file will take the preceding options. First, you configure\n");
  printf("properties like input rectangle, frame size etc. and the image file\n");
  printf("comes last.\n\n");

  printf("Encode options:\n");
  printf("  -r, --rect x,y,w,h      Limit next input image to given rectangle.\n");
  printf("  -f, --format FORMAT     Select encoding color format.\n");
  printf("                          Supported formats:\n");
  printf("                          ci4     4-bit color indexed\n");
  printf("                          ci8     8-bit color indexed\n");
  printf("                          i4      4-bit grayscale (intensity)\n");
  printf("                          i8      8-bit grayscale (intensity)\n");
  printf("                          rgba16  5-bit red, 5-bit green, 5-bit blue,\n");
  printf("                                  1-bit alpha\n");
  printf("                          rgb24   8-bit for red, green and blue\n");
  printf("                          rgba32  8-bit for red, green, blue and\n");
  printf("                                  alpha (default)\n");
  printf("                          ci4 and ci8 are not supported for palettes.\n");
  printf("                          When using ci4 or ci8, a palette with\n");
  printf("                          matching colors must be defined first.\n");
  printf("  -s, --size w,h          Set frame size for following images\n");
  printf("  -p, --pal FILE,x[,y]    Insert palette from FILE with given number\n");
  printf("                          of colors in the x and y direction.\n");
  printf("  -i, --image FILE[,w,h]  Encode given image file as a frameset.\n");
  printf("                          If frame size is defined, this image is\n");
  printf("                          split into multiple frames.\n");
  printf("  -t, --tiles N           The first N frames are used as tiles.\n");
  printf("                          Additional frames can be used for\n");
  printf("                          animations.\n");
  printf("  -a, --anim ANIM         Define an animation. ANIM is a series of\n");
  printf("                          0-based frame indices, for example:\n");
  printf("                          -a 0,1,2,1\n");
  printf("  --tile T,A              Define a tile animation.\n");
  printf("                          T is the 0-based tile to animate.\n");
  printf("                          A is the 0-based animation to apply.\n");
  printf("  --font FILE             Import font from FILE (BMFont format).\n");
  printf("                          The given file should refer to a PNG file\n");
  printf("                          to use as a texture.\n");
  printf("                          Rect definition are ignored for this\n");
  printf("                          texture.\n\n");
  printf("  -o, --output FILE       Write GFX file to FILE\n\n");

  printf("Examples:\n");
  printf("  gfxtool encode -i input.png -o output.gfx\n");
  printf("    Convert input.png to output.gfx.\n\n");
  printf("  gfxtool encode -i input.png,32,32 -o output.gfx\n");
  printf("    Convert input.png with 32x32 pixels frames to output.gfx.\n\n");
  printf("  gfxtool encode -s 8,8 -i tileset.png -t 32 -o output.gfx\n");
  printf("    Convert tileset.png to output.gfx with 32 8x8 tiles\n\n");
  printf("  gfxtool encode -r 0,0,64,8 -p input.png,8\n");
  printf("  -r 0,0,320,32 -f ci4 -i input.png,32,32 -o output.gfx\n");
  printf("    Convert input.png using color palette from same file.\n");
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    printUsage();
    return 0;
  }

  const char *command = argv[1];

  if (strcmp("encode", command) == 0) {
    return encode(argc - 2, argv + 2);
  }

  if (strcmp("decode", command) == 0) {
    printf("decode not implemented\n");
    return 1;
  }

  printf("Unknown command\n");
  return 1;
}
