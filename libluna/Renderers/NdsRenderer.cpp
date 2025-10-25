#include <libluna/Renderers/NdsRenderer.hpp>

#include <nds.h>

#include <libluna/Canvas.hpp>

using namespace Luna;

class NdsRenderer::impl {
  public:
  std::shared_ptr<Internal::GraphicsMetrics> mMetrics;
};

NdsRenderer::NdsRenderer() : mImpl{std::make_unique<impl>()} {}

NdsRenderer::~NdsRenderer() = default;

void NdsRenderer::initialize() { videoSetMode(MODE_0_2D); }

void NdsRenderer::initializeImmediateGui() {}

void NdsRenderer::quitImmediateGui() {}

void NdsRenderer::close() {}

void NdsRenderer::render() {
  auto canvas = getCanvas();
  auto bgColor = canvas->getBackgroundColor();

  setBackdropColor(makeColorUint16(bgColor));

  if (!canvas->getCamera2d().getStage()) {
    return;
  }
}

void NdsRenderer::present() { swiWaitForVBlank(); }

Internal::GraphicsMetrics NdsRenderer::getMetrics() { return *mImpl->mMetrics; }

void NdsRenderer::uploadTexture([[maybe_unused]] int slot, [[maybe_unused]] const Texture* texture) {
  // stub
}

void NdsRenderer::freeTexture([[maybe_unused]] int slot) {
  // stub
}
