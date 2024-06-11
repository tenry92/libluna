#include <libluna/config.h>

#ifdef _NDS
#include <libluna/Renderers/NdsRenderer.hpp>

#include <list>
#include <map>

#include <nds.h>

#include <libluna/Application.hpp>
#include <libluna/ImageResource.hpp>
#include <libluna/Logger.hpp>

#include <libluna/Canvas_impl.hpp>

using namespace Luna;

// https://mtheall.com/banks.html
// https://mtheall.com/vram.html

/*
 * Image data is encoded in 4bpp, where half a byte represents a color index
 * (16 possible colors per pixel).
 *
 * A sprite can be as small as 8x8 pixels (32 bytes) or
 * up to 64x64 pixels (2048 bytes).
 *
 * A single tile of a tileset is 8x8 pixels (4bpp = 32 bytes, 8bpp = 64 bytes).
 * If a tile is encoded as 8bpp, a tile has access to the full 256 colors of the
 * background palette. Else, each tile can choose one of the 16 palettes.
 *
 * A tile on a tilemap is encoded in 16 bits and supports hfliv, vflip and color
 * palette (if in 4bpp mode).
 *
 * In order to display 32 sprites à 16x16 pixels, 4096 bytes are required.
 * In order to store up to 1024 8x8 tiles (4bpp), 32'768 bytes are required.
 * In order to store a tilemap 512x256 pixels, 4096 bytes are required.
 *
 * The big banks (A, B, C, D) contain 128 KiB of memory.
 * These banks can hold up to 64 times 64x64 sprite data each.
 *
 * For a normal setup with 4 distinct tile layers (sharing the same 1024 tiles),
 * the following mapping is suggested:
 *
 * - VRAM_A_MAIN_BG_0x06000000
 * - VRAM_B_MAIN_SPRITE_0x06420000
 * - VRAM_C_SUB_BG_0x06200000
 * - VRAM_D_SUB_SPRITE
 *
 * For 3D, at least one of the big banks need to be mapped for 3D texture.
 * All four banks combined can hold a single 4bpp texture 1024x1024 in size.
 */

class NdsRenderer::impl {
  public:
};

NdsRenderer::NdsRenderer() : mImpl{std::make_unique<impl>()} {}

NdsRenderer::~NdsRenderer() = default;

void NdsRenderer::initialize() {
  // todo: am I main or am I sub?
  // modes: normal (sprite and tile layers), raw bitmap, 3D

  // main
  videoSetMode(MODE_0_2D);
  vramSetBankA(VRAM_A_MAIN_SPRITE_0x06400000);
  bgInit(0, BgType_Text4bpp, BgSize_T_512x256, 0, 1);
  bgInit(1, BgType_Text4bpp, BgSize_T_512x256, 2, 1);
  bgInit(2, BgType_Text4bpp, BgSize_T_512x256, 4, 1);
  bgInit(3, BgType_Text4bpp, BgSize_T_512x256, 6, 1);

  // 1D byte boundary offset: 32, 64, 128, 256
  // having 4 bpp, an 8x8 sprite takes 32 bytes
  oamInit(&oamMain, SpriteMapping_1D_32, false);

  // sub
  videoSetModeSub(MODE_0_2D);
  bgInitSub(0, BgType_Text4bpp, BgSize_T_512x256, 0, 1);
  bgInitSub(1, BgType_Text4bpp, BgSize_T_512x256, 2, 1);
  bgInitSub(2, BgType_Text4bpp, BgSize_T_512x256, 4, 1);
  bgInitSub(3, BgType_Text4bpp, BgSize_T_512x256, 6, 1);
}

void NdsRenderer::initializeImmediateGui() { return; }

void NdsRenderer::close() {
  // stub
}

void NdsRenderer::render() {
  auto canvas = getCanvas();

  if (!canvas || !canvas->getStage()) {
    return;
  }

  // new sprite: u16 *gfx = oamAllocateGfx(&oamMain, SpriteSize_16_16,
  // SpriteColorFormat, SpriteColorFormat_16Color); oamSet(&oamMain, oamIndex,
  // x, y, priority, palIndex, size, format, gfx, -1, false, ...); delete
  // sprite: oamFreeGfx(&oamMain, gfx); SPRITE_PALETTE[i] = RGB15(r, g, b);

  // at end: oamUpdate(&oamMain);

  // todo
}

void NdsRenderer::present() { return; }

#endif
