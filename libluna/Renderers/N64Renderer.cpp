#ifdef N64

#include <map>

#include <GL/gl.h>
#include <GL/gl_integration.h>
#include <GL/glu.h>
#include <libdragon.h>
#include <rspq_profile.h>

#include <libluna/Math.hpp>
#include <libluna/Renderers/N64Renderer.hpp>

using namespace Luna;

class N64Renderer::impl {
  public:
  std::shared_ptr<Internal::GraphicsMetrics> mMetrics;

  std::map<int, GLuint> mTextureIdMapping;
  std::map<int, GLuint> mMeshIdMapping;
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

void N64Renderer::endRender() { gl_context_end(); }

void N64Renderer::initialize() { gl_init(); }

void N64Renderer::initializeImmediateGui() {}

void N64Renderer::quitImmediateGui() {}

void N64Renderer::close() {}

void N64Renderer::present() { rdpq_detach_show(); }

Internal::GraphicsMetrics N64Renderer::getMetrics() { return *mImpl->mMetrics; }

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

void N64Renderer::loadTexture(
    [[maybe_unused]] int id, [[maybe_unused]] ImagePtr image
) {
  GLuint texture = mImpl->mTextureIdMapping.at(id);

  GLenum inputFormat = GL_RGBA;
  GLenum inputType = GL_UNSIGNED_SHORT_5_5_5_1_EXT;
  GLenum internalFormat = GL_RGB5_A1;

  if (image->getBitsPerPixel() == 24) {
    inputFormat = GL_RGB;
    inputType = GL_BYTE;
  } else if (image->getBitsPerPixel() == 32) {
    inputType = GL_UNSIGNED_INT_8_8_8_8_EXT;
    internalFormat = GL_RGBA;
  }

  glBindTexture(GL_TEXTURE_2D, texture);
  // note: on N64, a texture size that's not a power of 2, must be clamped
  if (!Math::isPowerOfTwo(image->getSize().x())) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  }

  if (!Math::isPowerOfTwo(image->getSize().y())) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  }

  glTexImage2D(
      GL_TEXTURE_2D, 0,                              /* mipmap level */
      internalFormat,                                /* internal format */
      image->getSize().x(), image->getSize().y(), 0, /* format (legacy) */
      inputFormat,                                   /* input format */
      inputType, image->getData()
  );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void N64Renderer::resizeTexture(
    [[maybe_unused]] int id, [[maybe_unused]] Vector2i size
) {}

void N64Renderer::renderTexture(
    [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderTextureInfo *info
) {
  GLuint texture = mImpl->mTextureIdMapping.at(info->textureId);

  float displayWidth = static_cast<float>(display_get_width());
  float displayHeight = static_cast<float>(display_get_height());

  float left = (info->position.x() / displayWidth * 2.0f) - 1.0f;
  float top = -(info->position.y() / displayHeight * 2.0f) + 1.0f;
  float right = ((info->position.x() + static_cast<float>(info->size.x())) /
                 displayWidth * 2.0f) -
                1.0f;
  float bottom = -((info->position.y() + static_cast<float>(info->size.y())) /
                   displayHeight * 2.0f) +
                 1.0f;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void N64Renderer::createMesh([[maybe_unused]] int id) {
  GLuint listId = glGenLists(1);
  mImpl->mMeshIdMapping.emplace(id, listId);
}

void N64Renderer::destroyMesh([[maybe_unused]] int id) {
  glDeleteLists(mImpl->mMeshIdMapping.at(id), 1);
  mImpl->mMeshIdMapping.erase(id);
}

void N64Renderer::loadMesh(
    [[maybe_unused]] int id, [[maybe_unused]] std::shared_ptr<Mesh> mesh
) {
  auto listId = mImpl->mMeshIdMapping.at(id);
  glNewList(listId, GL_COMPILE);

  glBegin(GL_TRIANGLES);
  for (std::size_t faceIndex = 0; faceIndex < mesh->getFaces().size();
       ++faceIndex) {
    for (std::size_t index : mesh->getFaces().at(faceIndex)) {
      auto &vertex = mesh->getVertices().at(index);
      auto &texCoords = mesh->getTexCoords().at(index);

      glTexCoord2f(texCoords.x(), texCoords.y());
      glVertex3f(vertex.x(), vertex.y(), vertex.z());
    }
  }
  glEnd();

  glEndList();
}

void N64Renderer::renderMesh(
    [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderMeshInfo *info
) {
  auto listId = mImpl->mMeshIdMapping.at(info->meshId);
  auto texture = mImpl->mTextureIdMapping.at(info->diffuseTextureId);

  auto ambientLight = canvas->getStage()->getAmbientLight();
  float ambient[4] = {
      ambientLight.color.red, ambientLight.color.green, ambientLight.color.blue,
      ambientLight.color.alpha
  };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  int lightId = 0;

  for (auto &&pointLight : canvas->getStage()->getPointLights()) {
    glEnable(GL_LIGHT0 + lightId);
    float pos[4] = {
        pointLight->position.x(), pointLight->position.y(),
        pointLight->position.z(), 1
    };
    glLightfv(GL_LIGHT0 + lightId, GL_POSITION, pos);
    float color[4] = {
        pointLight->color.red, pointLight->color.green, pointLight->color.blue,
        pointLight->color.alpha
    };
    glLightfv(GL_LIGHT0 + lightId, GL_DIFFUSE, color);
    float lightRadius = 10.0f;
    glLightf(GL_LIGHT0 + lightId, GL_LINEAR_ATTENUATION, 2.0f / lightRadius);
    glLightf(
        GL_LIGHT0 + lightId, GL_QUADRATIC_ATTENUATION,
        1.0f / (lightRadius * lightRadius)
    );
    ++lightId;
  }

  glEnable(GL_LIGHTING);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glBindTexture(GL_TEXTURE_2D, texture);

  auto camera = canvas->getCamera3d();

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf((camera.getProjectionMatrix(4.0f / 3.0f) *
                 camera.getViewMatrix())
                    .getValuePointer());

  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(info->transform.getValuePointer());

  glCallList(listId);
}

void N64Renderer::setTextureFilterEnabled(
    [[maybe_unused]] int id, [[maybe_unused]] bool enabled
) {}

void N64Renderer::setRenderTargetTexture([[maybe_unused]] int id) {}

void N64Renderer::unsetRenderTargetTexture() {}

void N64Renderer::setViewport(Vector2i offset, Vector2i size) {
  glViewport(offset.x(), offset.y(), size.x(), size.y());
}

void N64Renderer::imguiNewFrame() {}

#endif
