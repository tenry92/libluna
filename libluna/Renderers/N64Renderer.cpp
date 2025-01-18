#ifdef N64

#include <map>
#include <vector>

#include <GL/gl.h>
#include <GL/gl_integration.h>
#include <GL/glu.h>
#include <libdragon.h>
#include <rspq_profile.h>

#include <libluna/Canvas.hpp>
#include <libluna/Math.hpp>
#include <libluna/Renderers/N64Renderer.hpp>

using namespace Luna;

N64Renderer::N64Renderer() {
  mMetrics = std::make_shared<Internal::GraphicsMetrics>();
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

Internal::GraphicsMetrics N64Renderer::getMetrics() { return *mMetrics; }

void N64Renderer::clearBackground([[maybe_unused]] ColorRgb color) {
  glClearColor(color.red, color.green, color.blue, color.alpha);
  // glClearColor(0.0f, 0.9f, 0.7f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool N64Renderer::sliceTexture(Image *image, std::vector<Image> &slices, Vector2i &sliceCount) {
  const int tmemSize = 4096;

  if (image->getByteCount() <= tmemSize) {
    return false;
  }

  Vector2i sliceSize = image->getSize();
  sliceSize.width = Math::previousPowerOfTwo(sliceSize.width);
  sliceSize.height = Math::previousPowerOfTwo(sliceSize.height);

  while (sliceSize.width * sliceSize.height * (image->getBitsPerPixel() / 4) / 2 > tmemSize) {
    if (sliceSize.width > sliceSize.height) {
      sliceSize.width /= 2;
    } else {
      sliceSize.height /= 2;
    }
  }

  slices = image->slice(sliceSize, sliceCount);

  return true;
}

void N64Renderer::createTexture([[maybe_unused]] int id) {
  GLuint texture;
  glGenTextures(1, &texture);
  mTextureIdMapping.emplace(id, texture);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void N64Renderer::destroyTexture([[maybe_unused]] int id) {
  GLuint texture = mTextureIdMapping.at(id);
  mTextureIdMapping.erase(id);
  glDeleteTextures(1, &texture);
}

void N64Renderer::loadTexture(
    [[maybe_unused]] int id, [[maybe_unused]] Image *image
) {
  auto &texture = mTextureIdMapping.at(id);

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

  auto imageSize = image->getSize();

  glBindTexture(GL_TEXTURE_2D, texture);
  // note: on N64, a texture size that's not a power of 2, must be clamped
  if (!Math::isPowerOfTwo(imageSize.width)) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  }

  if (!Math::isPowerOfTwo(imageSize.height)) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  }

  glTexImage2D(
      GL_TEXTURE_2D, 0, /* mipmap level */
      internalFormat,   /* internal format */
      image->getWidth(), image->getHeight(),
      0,           /* format (legacy) */
      inputFormat, /* input format */
      inputType, image->getData()
  );
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
      image->isInterpolated() ? GL_LINEAR : GL_NEAREST
  );
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
      image->isInterpolated() ? GL_LINEAR : GL_NEAREST
  );
}

void N64Renderer::resizeTexture(
    [[maybe_unused]] int id, [[maybe_unused]] Vector2i size
) {}

void N64Renderer::renderTexture(
    [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderTextureInfo *info
) {
  auto texture = mTextureIdMapping.at(info->textureId);

  float displayWidth = static_cast<float>(display_get_width());
  float displayHeight = static_cast<float>(display_get_height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, displayWidth, displayHeight, 0.0, -1.0, 1.0);

  Vector2f uvTopLeft(0.0f, 0.0f);
  Vector2f uvBottomRight(1.0f, 1.0f);

  if (info->crop.area() > 0) {
    uvTopLeft.x = static_cast<float>(info->crop.x % info->size.width) /
                  static_cast<float>(info->size.width);

    uvBottomRight.x =
        static_cast<float>(
            (info->crop.x + info->crop.width - 1) % info->size.width + 1
        ) /
        static_cast<float>(info->size.width);

    uvTopLeft.y = static_cast<float>(info->crop.y % info->size.height) /
                  static_cast<float>(info->size.height);

    uvBottomRight.y =
        static_cast<float>(
            (info->crop.y + info->crop.height - 1) % info->size.height + 1
        ) /
        static_cast<float>(info->size.height);
  }

  float left = info->position.x;
  float top = info->position.y;
  float right = info->position.x + static_cast<float>(info->size.width);
  float bottom = info->position.y + static_cast<float>(info->size.height);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(uvTopLeft.x, uvBottomRight.y);
  glVertex3f(left, bottom, 0.0f);

  glTexCoord2f(uvTopLeft.x, uvTopLeft.y);
  glVertex3f(left, top, 0.0f);

  glTexCoord2f(uvBottomRight.x, uvTopLeft.y);
  glVertex3f(right, top, 0.0f);

  glTexCoord2f(uvBottomRight.x, uvBottomRight.y);
  glVertex3f(right, bottom, 0.0f);
  glEnd();
}

void N64Renderer::createShape([[maybe_unused]] int id) {}

void N64Renderer::destroyShape([[maybe_unused]] int id) {
  mShapeIdMapping.erase(id);
}

void N64Renderer::loadShape(
    [[maybe_unused]] int id, [[maybe_unused]] Shape *shape
) {
  mShapeIdMapping.emplace(id, shape);
}

void N64Renderer::renderShape(
    [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderShapeInfo *info
) {
  auto shape = mShapeIdMapping.at(info->shapeId);

  float displayWidth = static_cast<float>(display_get_width());
  float displayHeight = static_cast<float>(display_get_height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, displayWidth, displayHeight, 0.0, -1.0, 1.0);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor3f(1.0f, 0.0f, 0.0f);
  glLineWidth(1.0f);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBegin(GL_LINE_STRIP);

  for (auto &&vertex : shape->getVertices()) {
    float x = info->position.x + vertex.x;
    float y = info->position.y + vertex.y;

    glVertex2f(x, y);
  }
  glEnd();

  glColor3f(1.0f, 1.0f, 1.0f);
}

void N64Renderer::createMesh([[maybe_unused]] int id) {
  GLuint listId = glGenLists(1);
  mMeshIdMapping.emplace(id, listId);
}

void N64Renderer::destroyMesh([[maybe_unused]] int id) {
  glDeleteLists(mMeshIdMapping.at(id), 1);
  mMeshIdMapping.erase(id);
}

void N64Renderer::loadMesh(
    [[maybe_unused]] int id, [[maybe_unused]] std::shared_ptr<Mesh> mesh
) {
  auto listId = mMeshIdMapping.at(id);
  glNewList(listId, GL_COMPILE);

  glBegin(GL_TRIANGLES);
  for (std::size_t faceIndex = 0; faceIndex < mesh->getFaces().size();
       ++faceIndex) {
    for (std::size_t index : mesh->getFaces().at(faceIndex)) {
      auto &vertex = mesh->getVertices().at(index);
      auto &texCoords = mesh->getTexCoords().at(index);

      glTexCoord2f(texCoords.x, texCoords.y);
      glVertex3f(vertex.x, vertex.y, vertex.z);
    }
  }
  glEnd();

  glEndList();
}

void N64Renderer::renderMesh(
    [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderMeshInfo *info
) {
  auto listId = mMeshIdMapping.at(info->meshId);
  auto texture = mTextureIdMapping.at(info->diffuseTextureId);

  auto ambientLight = canvas->getStage()->getAmbientLight();
  float ambient[4] = {
      ambientLight.color.red, ambientLight.color.green, ambientLight.color.blue,
      ambientLight.color.alpha};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  int lightId = 0;

  for (auto &&pointLight : canvas->getStage()->getPointLights()) {
    glEnable(GL_LIGHT0 + lightId);
    float pos[4] = {
        pointLight->position.x, pointLight->position.y, pointLight->position.z,
        1};
    glLightfv(GL_LIGHT0 + lightId, GL_POSITION, pos);
    float color[4] = {
        pointLight->color.red, pointLight->color.green, pointLight->color.blue,
        pointLight->color.alpha};
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
  glViewport(offset.x, offset.y, size.width, size.height);
}

void N64Renderer::imguiNewFrame() {}

#endif
