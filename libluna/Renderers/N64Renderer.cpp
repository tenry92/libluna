#ifdef N64

#include <map>

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

  std::map<int, GLuint> mTextureIdMapping;
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

void N64Renderer::createTexture([[maybe_unused]] int id) {
  GLuint texture;
  glGenTextures(1, &texture);
  mImpl->mTextureIdMapping.emplace(id, texture);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void N64Renderer::destroyTexture([[maybe_unused]] int id) {
  GLuint texture = mImpl->mTextureIdMapping.at(id);
  mImpl->mTextureIdMapping.erase(id);
  glDeleteTextures(1, &texture);
}

void N64Renderer::loadTexture([[maybe_unused]] int id, [[maybe_unused]] ImagePtr image) {
  GLuint texture = mImpl->mTextureIdMapping.at(id);

  GLenum inputFormat = GL_RGBA;
  GLenum inputType = GL_UNSIGNED_SHORT_5_5_5_1_EXT;

  if (image->getBitsPerPixel() == 24) {
    inputFormat = GL_RGB;
    inputType = GL_BYTE;
  } else if (image->getBitsPerPixel() == 32) {
    inputType = GL_UNSIGNED_INT_8_8_8_8_EXT;
  }

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(
      GL_TEXTURE_2D, 0,                              /* mipmap level */
      GL_RGB5_A1,                                    /* internal format */
      image->getSize().x(), image->getSize().y(), 0, /* format (legacy) */
      inputFormat,                                   /* input format */
      inputType, image->getData()
  );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void N64Renderer::resizeTexture([[maybe_unused]] int id, [[maybe_unused]] Vector2i size) {}

void N64Renderer::renderTexture([[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderTextureInfo *info) {
  GLuint texture = mImpl->mTextureIdMapping.at(info->textureId);

  float displayWidth = static_cast<float>(display_get_width());
  float displayHeight = static_cast<float>(display_get_height());

  float left = (info->position.x() / displayWidth * 2.0f) - 1.0f;
  float top = (info->position.y() / displayHeight * 2.0f) - 1.0f;
  float right = ((info->position.x() + static_cast<float>(info->size.x())) / displayWidth * 2.0f) - 1.0f;
  float bottom = ((info->position.y() - static_cast<float>(info->size.y())) / displayHeight * 2.0f) - 1.0f;

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(left, bottom, 0.0f);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(left, top, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(right, top, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(right, bottom, 0.0f);
  glEnd();
}

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
