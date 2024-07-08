#ifdef N64

#include <libdragon.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>
#include <rspq_profile.h>

#include <libluna/Renderers/N64Renderer.hpp>

using namespace Luna;

class N64Renderer::impl {
  public:
  std::shared_ptr<Internal::GraphicsMetrics> mMetrics;
};

N64Renderer::N64Renderer() : mImpl{std::make_unique<impl>()} {
  mImpl->mMetrics = std::make_shared<Internal::GraphicsMetrics>();
}

N64Renderer::~N64Renderer() = default;

void N64Renderer::startRender() {
  surface_t *disp = display_get();
  surface_t *zbuf = display_get_zbuf();

  rdpq_attach(disp, zbuf);

  gl_context_begin();
}

void N64Renderer::endRender() {
  gl_context_end();
}

void N64Renderer::initialize() {
  gl_init();
}

void N64Renderer::initializeImmediateGui() {}

void N64Renderer::quitImmediateGui() {}

void N64Renderer::close() {}

void N64Renderer::present() {
  rdpq_detach_show();
}

Internal::GraphicsMetrics N64Renderer::getMetrics() {
  return *mImpl->mMetrics;
}

void N64Renderer::clearBackground([[maybe_unused]] ColorRgb color) {
  glClearColor(color.red, color.green, color.blue, color.alpha);
  // glClearColor(0.0f, 0.9f, 0.7f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void N64Renderer::createTexture([[maybe_unused]] int id) {}

void N64Renderer::destroyTexture([[maybe_unused]] int id) {}

void N64Renderer::loadTexture([[maybe_unused]] int id, [[maybe_unused]] ImagePtr image) {}

void N64Renderer::resizeTexture([[maybe_unused]] int id, [[maybe_unused]] Vector2i size) {}

void N64Renderer::renderTexture([[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderTextureInfo *info) {}

void N64Renderer::createMesh([[maybe_unused]] int id) {}

void N64Renderer::destroyMesh([[maybe_unused]] int id) {}

void N64Renderer::loadMesh([[maybe_unused]] int id, [[maybe_unused]] std::shared_ptr<Mesh> mesh) {}

void N64Renderer::renderMesh([[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderMeshInfo *info) {}

void N64Renderer::setTextureFilterEnabled([[maybe_unused]] int id, [[maybe_unused]] bool enabled) {}

void N64Renderer::setRenderTargetTexture([[maybe_unused]] int id) {}

void N64Renderer::unsetRenderTargetTexture() {}

void N64Renderer::setViewport(Vector2i offset, Vector2i size) {
  glViewport(offset.x(), offset.y(), size.x(), size.y());
}

void N64Renderer::imguiNewFrame() {}

#endif
