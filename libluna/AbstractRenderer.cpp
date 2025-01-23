#include <libluna/AbstractRenderer.hpp>

using namespace Luna;

AbstractRenderer::AbstractRenderer() = default;

AbstractRenderer::~AbstractRenderer() = default;

void AbstractRenderer::setCanvas(Canvas* canvas) { mCanvas = canvas; }

Canvas* AbstractRenderer::getCanvas() const { return mCanvas; }
