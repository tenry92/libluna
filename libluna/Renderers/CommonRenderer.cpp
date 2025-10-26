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

  renderWorld(canvas);

#ifndef N64
  if (!mRenderTargetId) {
    mRenderTargetId = mTextureIdAllocator.next();
    logDebug("create texture #{} (render target)", mRenderTargetId);
    createFramebufferTexture(mRenderTargetId, getInternalSize());
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

void CommonRenderer::declareGpuTexture(
  int slot, GpuTexture& texture
) {
  if (texture.subTextures.empty()) {
    texture.id = mTextureIdAllocator.next();
    logDebug("declare texture #{} in slot {} (size: {}x{})",
      texture.id, slot, texture.size.width, texture.size.height
    );
  } else {
    texture.id = 0; // Uses sub textures

    for (auto& subTexture : texture.subTextures) {
      subTexture.id = mTextureIdAllocator.next();
      logDebug("declare sub texture #{} in slot {} (crop: {}x{}+{}+{})",
        subTexture.id, slot,
        subTexture.crop.width, subTexture.crop.height,
        subTexture.crop.x, subTexture.crop.y
      );
    }
  }

  mGpuTextureSlotMapping[slot] = texture;
}

CommonRenderer::GpuTexture* CommonRenderer::getGpuTexture(int slot) {
  if (mGpuTextureSlotMapping.find(slot) == mGpuTextureSlotMapping.end()) {
    return nullptr;
  }

  return &mGpuTextureSlotMapping.at(slot);
}

void CommonRenderer::freeGpuTexture(int slot) {
  if (mGpuTextureSlotMapping.find(slot) == mGpuTextureSlotMapping.end()) {
    return;
  }

  auto gpuTexture = mGpuTextureSlotMapping.at(slot);

  if (gpuTexture.id != 0) {
    logDebug("free texture #{} from slot {}", gpuTexture.id, slot);
    mTextureIdAllocator.free(gpuTexture.id);
  } else {
    for (auto& subTexture : gpuTexture.subTextures) {
      logDebug("free sub texture #{} from slot {}", subTexture.id, slot);
      mTextureIdAllocator.free(subTexture.id);
    }
  }

  mGpuTextureSlotMapping.erase(slot);
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

bool CommonRenderer::sliceTexture(
  [[maybe_unused]] Texture* texture, [[maybe_unused]] std::vector<Texture>& slices,
  [[maybe_unused]] Vector2i& sliceCount
) {
  return false;
}

void CommonRenderer::createShape([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::destroyShape([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::loadShape(
  [[maybe_unused]] int id, [[maybe_unused]] Shape* shape
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
  [[maybe_unused]] Canvas* canvas, [[maybe_unused]] RenderMeshInfo* info
) {
  // stub
}

void CommonRenderer::renderTexture(
  [[maybe_unused]] Canvas* canvas, [[maybe_unused]] RenderTextureInfo* info
) {
  // stub
}

void CommonRenderer::renderShape(
  [[maybe_unused]] Canvas* canvas, [[maybe_unused]] RenderShapeInfo* info
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
  [[maybe_unused]] uint16_t id, [[maybe_unused]] bool enabled
) {
  // stub
}

void CommonRenderer::setRenderTargetTexture([[maybe_unused]] uint16_t id) {
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
  return getCanvas()->getInternalResolution();
}

Vector2i CommonRenderer::getCurrentRenderSize() const {
  return mCurrentRenderSize;
}

Vector2i CommonRenderer::getTextureSize(int slot) const {
  return mGpuTextureSlotMapping.at(slot).size;
}

void CommonRenderer::renderWorld(Canvas* canvas) {
  auto stage = canvas->getCamera3d().getStage();

  if (!stage) {
    return;
  }

  for (auto&& model : stage->getDrawables3d()) {
    RenderMeshInfo info;
    info.meshId = mKnownMeshes.at(model->getMesh());
    info.transform = model->getTransform();

    auto material = model->getMaterial();

    int diffuseTextureSlot = material.getDiffuseTexture();
    if (diffuseTextureSlot != 0 && mGpuTextureSlotMapping.find(diffuseTextureSlot) != mGpuTextureSlotMapping.end()) {
      auto& gpuTexture = mGpuTextureSlotMapping.at(diffuseTextureSlot);
      info.diffuseTextureId = gpuTexture.id;
    }

    int normalTextureSlot = material.getNormalTexture();
    if (normalTextureSlot != 0 && mGpuTextureSlotMapping.find(normalTextureSlot) != mGpuTextureSlotMapping.end()) {
      auto& gpuTexture = mGpuTextureSlotMapping.at(normalTextureSlot);
      info.normalTextureId = gpuTexture.id;
    }

    renderMesh(canvas, &info);
  }
}

void CommonRenderer::render2d(
  Canvas* canvas, [[maybe_unused]] Vector2i renderSize
) {
  auto stage = canvas->getCamera2d().getStage();

  if (!stage) {
    return;
  }

  for (auto&& drawable : stage->getSortedDrawables2d()) {
    std::visit(
      overloaded{
        [](auto) {},
        [&](const Sprite& sprite) {
          if (!sprite.isVisible()) {
            return;
          }

          int textureSlot = sprite.getTexture();

          if (mGpuTextureSlotMapping.find(textureSlot) == mGpuTextureSlotMapping.end()) {
            return;
          }

          RenderTextureInfo info;
          auto& gpuTexture = mGpuTextureSlotMapping.at(textureSlot);

          info.gpuTexture = &gpuTexture;
          info.textureId = gpuTexture.id;
          info.size = gpuTexture.size;
          info.position =
            sprite.getPosition() - canvas->getCamera2d().getPosition();
          renderTexture(canvas, &info);
        },
        [&](const Primitive& primitive) {
          if (!primitive.getShape() || !mKnownShapes.count(primitive.getShape())) {
            return;
          }

          RenderShapeInfo info;
          info.shapeId = mKnownShapes.at(primitive.getShape());
          info.position = primitive.getPosition();
          renderShape(canvas, &info);
        },
        [&](const Tilemap& tilemap) {
          auto tileset = tilemap.getTileset();
          int textureSlot = tileset->getTextureId();

          if (textureSlot == 0 || mGpuTextureSlotMapping.find(textureSlot) == mGpuTextureSlotMapping.end()) {
            // Texture not uploaded
            return;
          }

          auto& gpuTexture = mGpuTextureSlotMapping.at(textureSlot);

          for (int y = 0; y < tilemap.getSize().height; ++y) {
            for (int x = 0; x < tilemap.getSize().width; ++x) {
              auto tile = tilemap.at({x, y});
              if (tile == 0) {
                continue;
              }

              RenderTextureInfo info;
              info.gpuTexture = &gpuTexture;
              info.textureId = gpuTexture.id;
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
        [&](const Text& text) {
          auto font = text.getFont();

          auto startPosition = text.getPosition();

          float x = startPosition.x;
          float y = startPosition.y;

          y += text.getSize() * static_cast<float>(font->getBaseLine());

          for (auto&& cp : text.getContent()) {
            if (cp == '\n') {
              x = startPosition.x;
              y +=
                text.getSize() * text.getLineHeight() * static_cast<float>(font->getLineHeight());
              continue;
            }

            auto glyph = font->getGlyphByCodePoint(cp);

            if (!glyph) {
              // unknown glyph
              continue;
            }

            if (cp != ' ' &&
                mGpuTextureSlotMapping.find(glyph->textureSlot) != mGpuTextureSlotMapping.end()) {
              RenderTextureInfo info;

              auto& gpuTexture = mGpuTextureSlotMapping.at(glyph->textureSlot);

              info.gpuTexture = &gpuTexture;
              info.textureId = gpuTexture.id;
              info.position =
                Vector2i(static_cast<int>(x), static_cast<int>(y)) +
                Vector2i(
                  static_cast<int>(text.getSize() * static_cast<float>(glyph->offset.x)),
                  static_cast<int>(text.getSize() * static_cast<float>(glyph->offset.y))
                );

              if (glyph->crop.area() > 0) {
                info.size = {
                  static_cast<int>(text.getSize() * static_cast<float>(glyph->crop.width)),
                  static_cast<int>(text.getSize() * static_cast<float>(glyph->crop.height))
                };
                info.crop = glyph->crop;
              } else {
                info.size = gpuTexture.size;
              }

              renderTexture(canvas, &info);
            }

            x += text.getSize() * static_cast<float>(glyph->advance);
          }
        }},
      *drawable
    );
  }
}

void CommonRenderer::start2dFramebuffer([[maybe_unused]] Canvas* canvas) {
  resizeFramebufferTexture(mRenderTargetId, getInternalSize());
  setRenderTargetTexture(mRenderTargetId);
  setViewport({0, 0}, getInternalSize());
  mCurrentRenderSize = getInternalSize();
  clearBackground(ColorRgb{0, 0, 0, 0});
}

void CommonRenderer::end2dFramebuffer([[maybe_unused]] Canvas* canvas) {
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
