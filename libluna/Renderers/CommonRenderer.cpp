#include <libluna/config.h>

#include <unordered_set>

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

#ifdef LUNA_WINDOW_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#ifdef LUNA_WINDOW_EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#ifdef __SWITCH__
#include <switch.h>
#endif

#ifdef N64
#include <libdragon.h>
#endif

#include <libluna/Canvas.hpp>
#include <libluna/Logger.hpp>
#include <libluna/Renderers/CommonRenderer.hpp>
#include <libluna/overloaded.hpp>

using namespace Luna;

CommonRenderer::CommonRenderer() : mRenderTargetId{0} {}

CommonRenderer::~CommonRenderer() {
  // todo
}

void CommonRenderer::render() {
  startRender();
  imguiNewFrame();

  setViewport({0, 0}, getCanvasSize());
  mCurrentRenderSize = getCanvasSize();

  auto canvas = getCanvas();

  auto bgColor = canvas->getBackgroundColor();
  clearBackground(bgColor);

  if (!canvas->getStage()) {
    return;
  }

  updateTextureCache(canvas->getStage());

  renderWorld(canvas);

#ifndef N64
  if (!mRenderTargetId) {
    mRenderTargetId = mTextureIdAllocator.next();
    logDebug("create texture #{} (render target)", mRenderTargetId);
    createTexture(mRenderTargetId);
  }

  // render sprites to internal texture
  start2dFramebuffer(canvas);
  // clearBackground({0.5f, 0.0f, 0.0f, 0.0f});
#endif
  render2d(canvas, getInternalSize());
#ifndef N64
  end2dFramebuffer(canvas);
#endif
  endRender();
}

void CommonRenderer::startRender() {
  // stub
}

void CommonRenderer::endRender() {
  // stub
}

void CommonRenderer::clearBackground([[maybe_unused]] ColorRgb color) {
  // stub
}

void CommonRenderer::createTexture([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::destroyTexture([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::loadTexture(
    [[maybe_unused]] int id, [[maybe_unused]] Image *image
) {
  // stub
}

void CommonRenderer::resizeTexture(
    [[maybe_unused]] int id, [[maybe_unused]] Vector2i size
) {
  // stub
}

void CommonRenderer::createShape([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::destroyShape([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::loadShape(
    [[maybe_unused]] int id, [[maybe_unused]] Shape *shape
) {
  // stub
}

void CommonRenderer::setViewport(
    [[maybe_unused]] Vector2i offset, [[maybe_unused]] Vector2i size
) {
  // stub
}

void CommonRenderer::imguiNewFrame() {
  // stub
}

void CommonRenderer::renderMesh(
    [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderMeshInfo *info
) {
  // stub
}

void CommonRenderer::renderTexture(
    [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderTextureInfo *info
) {
  // stub
}

void CommonRenderer::renderShape(
    [[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderShapeInfo *info
) {
  // stub
}

void CommonRenderer::createMesh([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::destroyMesh([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::loadMesh(
    [[maybe_unused]] int id, [[maybe_unused]] std::shared_ptr<Mesh> mesh
) {
  // stub
}

void CommonRenderer::setTextureFilterEnabled(
    [[maybe_unused]] int id, [[maybe_unused]] bool enabled
) {
  // stub
}

void CommonRenderer::setRenderTargetTexture([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::unsetRenderTargetTexture() {
  // stub
}

Vector2i CommonRenderer::getCanvasSize() const {
  int canvasWidth = 1920;
  int canvasHeight = 1080;

#ifdef LUNA_WINDOW_SDL2
  SDL_GetWindowSize(getCanvas()->sdl.window, &canvasWidth, &canvasHeight);
#endif
#ifdef LUNA_WINDOW_GLFW
  glfwGetFramebufferSize(getCanvas()->glfw.window, &canvasWidth, &canvasHeight);
#endif
#ifdef __SWITCH__
  ViDisplay display;
  viOpenDefaultDisplay(&display);
  viGetDisplayResolution(&display, &canvasWidth, &canvasHeight);
#endif
#ifdef N64
  canvasWidth = display_get_width();
  canvasHeight = display_get_height();
#endif

  return Vector2i(canvasWidth, canvasHeight);
}

Vector2i CommonRenderer::getInternalSize() const {
  return getCanvas()->getOriginalSize();
}

Vector2i CommonRenderer::getCurrentRenderSize() const {
  return mCurrentRenderSize;
}

Vector2i CommonRenderer::getTextureSize(int id) const {
  return mTextureIdMapping.at(id).size;
}

void CommonRenderer::updateTextureCache(
    [[maybe_unused]] Stage *stage
) {
  auto imagesLoaderCache = stage->getTextureCache()->getCache();

  std::unordered_set<ImageLoader *> visitedImageLoaders;

  for (auto &&[imageLoader, priority] : imagesLoaderCache) {
    visitedImageLoaders.emplace(imageLoader);
    if (mKnownImages.count(imageLoader) == 0) {
      auto image = imageLoader->load();

      int textureId = mTextureIdAllocator.next();
      mKnownImages.emplace(imageLoader, Texture{textureId, image.getSize()});
      mTextureIdMapping.emplace(
          textureId, Texture{textureId, image.getSize()}
      );
      logDebug("create texture #{}", textureId);
      createTexture(textureId);
      loadTexture(textureId, &image);
    }
  }

  for (auto it = mKnownImages.begin(); it != mKnownImages.end();) {
    if (visitedImageLoaders.find(it->first) == visitedImageLoaders.end()) {
      logDebug("destroy texture #{}", it->second.id);
      mTextureIdAllocator.free(static_cast<uint16_t>(it->second.id));
      destroyTexture(it->second.id);
      it = mKnownImages.erase(it);
      mTextureIdMapping.erase(it->second.id);
    } else {
      ++it;
    }
  }

  std::unordered_set<Shape *> visitedShapes;

  for (auto &&drawable : stage->getDrawables2d()) {
    if (!std::holds_alternative<Primitive>(drawable)) {
      continue;
    }

    auto &primitive = std::get<Primitive>(drawable);

    if (primitive.getShape() == nullptr) {
      continue;
    }

    visitedShapes.emplace(primitive.getShape());

    if (mKnownShapes.count(primitive.getShape()) == 0) {
      int shapeId = mShapeIdAllocator.next();
      mKnownShapes.emplace(primitive.getShape(), shapeId);
      createShape(shapeId);
      loadShape(shapeId, primitive.getShape());
    }
  }

  for (auto it = mKnownShapes.begin(); it != mKnownShapes.end();) {
    if (visitedShapes.find(it->first) == visitedShapes.end()) {
      mShapeIdAllocator.free(static_cast<uint16_t>(it->second));
      destroyShape(it->second);
      it = mKnownShapes.erase(it);
    } else {
      ++it;
    }
  }

  std::unordered_set<std::shared_ptr<Mesh>> visitedMeshes;

  for (auto &&model : stage->getDrawables3d()) {
    visitedMeshes.emplace(model->getMesh());

    if (mKnownMeshes.count(model->getMesh()) == 0) {
      int meshId = mMeshIdAllocator.next();
      mKnownMeshes.emplace(model->getMesh(), meshId);
      createMesh(meshId);
      loadMesh(meshId, model->getMesh());
    }
  }
}

void CommonRenderer::renderWorld(Canvas *canvas) {
  for (auto &&model : canvas->getStage()->getDrawables3d()) {
    RenderMeshInfo info;
    info.meshId = mKnownMeshes.at(model->getMesh());
    info.transform = model->getTransform();

    auto material = model->getMaterial();

    if (material.getDiffuse()) {
      info.diffuseTextureId = mKnownImages.at(material.getDiffuse()).id;
    }

    if (material.getNormal()) {
      info.normalTextureId = mKnownImages.at(material.getNormal()).id;
    }

    renderMesh(canvas, &info);
  }
}

void CommonRenderer::render2d(
    Canvas *canvas, [[maybe_unused]] Vector2i renderSize
) {
  for (auto &&drawable : canvas->getStage()->getSortedDrawables2d()) {
    std::visit(
        overloaded{
            [](auto) {},
            [&](const Sprite &sprite) {
              if (!sprite.isVisible()) {
                return;
              }

              if (!sprite.getImageLoader() || !mKnownImages.count(sprite.getImageLoader())) {
                return;
              }

              RenderTextureInfo info;
              auto texture = mKnownImages.at(sprite.getImageLoader());
              info.textureId = texture.id;
              info.size = texture.size;
              info.position =
                  sprite.getPosition() - canvas->getCamera2d().getPosition();
              renderTexture(canvas, &info);
            },
            [&](const Primitive &primitive) {
              if (!primitive.getShape() || !mKnownShapes.count(primitive.getShape())) {
                return;
              }

              RenderShapeInfo info;
              info.shapeId = mKnownShapes.at(primitive.getShape());
              info.position = primitive.getPosition();
              renderShape(canvas, &info);
            },
            [&](const Tilemap &tilemap) {
              auto tileset = tilemap.getTileset();
              auto texture = mKnownImages.at(tileset->getImage());

              for (int y = 0; y < tilemap.getSize().height; ++y) {
                for (int x = 0; x < tilemap.getSize().width; ++x) {
                  auto tile = tilemap.at({x, y});
                  if (tile == 0) {
                    continue;
                  }

                  RenderTextureInfo info;
                  info.textureId = texture.id;
                  info.crop = {
                      tile % tileset->getColumns() * tileset->getTileSize(),
                      tile / tileset->getColumns() * tileset->getTileSize(),
                      tileset->getTileSize(), tileset->getTileSize()};
                  info.size = {tileset->getTileSize(), tileset->getTileSize()};
                  info.position = {
                      static_cast<float>(x * tileset->getTileSize()),
                      static_cast<float>(y * tileset->getTileSize())};
                  renderTexture(canvas, &info);
                }
              }
            },
            [&](const Text &text) {
              auto font = text.getFont();

              int x = 0;
              int y = font->getBaseLine();

              for (auto &&cp : text.getContent()) {
                if (cp == '\n') {
                  x = 0;
                  y += font->getLineHeight();
                  continue;
                }

                auto glyph = font->getGlyphByCodePoint(cp);

                if (!glyph) {
                  // unknown glyph
                  continue;
                }

                if (cp != ' ' && glyph->imageLoader) {
                  RenderTextureInfo info;
                  auto texture = mKnownImages.at(glyph->imageLoader);

                  info.textureId = texture.id;
                  info.position = Vector2i(x, y) + glyph->offset;

                  if (glyph->crop.area() > 0) {
                    info.size = {glyph->crop.width, glyph->crop.height};
                    info.crop = glyph->crop;
                  } else {
                    info.size = texture.size;
                  }

                  renderTexture(canvas, &info);
                }

                x += glyph->advance;
              }
            }},
        drawable
    );
  }
}

void CommonRenderer::start2dFramebuffer([[maybe_unused]] Canvas *canvas) {
  resizeTexture(mRenderTargetId, getInternalSize());
  setRenderTargetTexture(mRenderTargetId);
  setViewport({0, 0}, getInternalSize());
  mCurrentRenderSize = getInternalSize();
  clearBackground(ColorRgb{0, 0, 0, 0});
}

void CommonRenderer::end2dFramebuffer([[maybe_unused]] Canvas *canvas) {
  unsetRenderTargetTexture();
  setViewport({0, 0}, getCanvasSize());
  mCurrentRenderSize = getCanvasSize();

  // scale internal texture to screen
  auto scaledSize = getInternalSize().scaleToFit(getCanvasSize());
  Vector2f pos{0, 0};

  if (getCanvasSize().width > scaledSize.width) {
    pos.x = static_cast<float>(getCanvasSize().width - scaledSize.width) / 2;
  } else {
    pos.y = static_cast<float>(getCanvasSize().height - scaledSize.height) / 2;
  }

  RenderTextureInfo info;
  info.textureId = mRenderTargetId;
  info.size = scaledSize;
  info.position = pos;
  renderTexture(canvas, &info);
}
