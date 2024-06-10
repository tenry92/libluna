#include <libluna/AbstractRenderer.hpp>

#include <libluna/CanvasImpl.hpp>

using namespace Luna;

class AbstractRenderer::impl {
  public:
  Canvas *mCanvas;
};

AbstractRenderer::AbstractRenderer() : mImpl{std::make_unique<impl>()} {}

AbstractRenderer::~AbstractRenderer() = default;

void AbstractRenderer::setCanvas(Canvas *canvas) { mImpl->mCanvas = canvas; }

Canvas *AbstractRenderer::getCanvas() const { return mImpl->mCanvas; }
