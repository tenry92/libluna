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

#include <libluna/Renderers/CommonRenderer.hpp>
#include <libluna/CanvasImpl.hpp>
#include <libluna/Logger.hpp>

using namespace Luna;

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

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

void CommonRenderer::loadTexture([[maybe_unused]] int id, [[maybe_unused]] ImagePtr image) {
  // stub
}

void CommonRenderer::resizeTexture([[maybe_unused]] int id, [[maybe_unused]] Vector2i size) {
  // stub
}

void CommonRenderer::setViewport([[maybe_unused]] Vector2i offset, [[maybe_unused]] Vector2i size) {
  // stub
}

void CommonRenderer::imguiNewFrame() {
  // stub
}

void CommonRenderer::renderMesh([[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderMeshInfo *info) {
  // stub
}

void CommonRenderer::renderTexture([[maybe_unused]] Canvas *canvas, [[maybe_unused]] RenderTextureInfo *info) {
  // stub
}

void CommonRenderer::createMesh([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::destroyMesh([[maybe_unused]] int id) {
  // stub
}

void CommonRenderer::loadMesh([[maybe_unused]] int id, [[maybe_unused]] std::shared_ptr<Mesh> mesh) {
  // stub
}

void CommonRenderer::setTextureFilterEnabled([[maybe_unused]] int id, [[maybe_unused]] bool enabled) {
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
  SDL_GetWindowSize(
      getCanvas()->getImpl()->sdl.window, &canvasWidth, &canvasHeight
  );
#endif
#ifdef LUNA_WINDOW_GLFW
  glfwGetFramebufferSize(
      getCanvas()->getImpl()->glfw.window, &canvasWidth, &canvasHeight
  );
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
  return getCanvas()->getImpl()->mOriginalSize;
}

Vector2i CommonRenderer::getCurrentRenderSize() const {
  return mCurrentRenderSize;
}

void CommonRenderer::updateTextureCache([[maybe_unused]] std::shared_ptr<Stage> stage) {
  auto images = listImagesInUse(stage);

  std::unordered_set<ImageResPtr> visitedImages;

  for (auto &&imageResPtr : images) {
    visitedImages.emplace(imageResPtr);
    if (mKnownImages.count(imageResPtr) == 0) {
      auto future = imageResPtr->get();
      // check with each update whether future is ready:
      // if (imageRef->isReady()):
      auto image = future.get();
      int textureId = mTextureIdAllocator.next();
      mKnownImages.emplace(imageResPtr, Texture{textureId, image->getSize()});
      logDebug("create texture #{}", textureId);
      createTexture(textureId);
      loadTexture(textureId, image);
    }
  }

  for (auto it = mKnownImages.begin(); it != mKnownImages.end();) {
    if (visitedImages.find(it->first) == visitedImages.end()) {
      logDebug("destroy texture #{}", it->second.id);
      mTextureIdAllocator.free(static_cast<uint8_t>(it->second.id));
      destroyTexture(it->second.id);
      it = mKnownImages.erase(it);
    } else {
      ++it;
    }
  }

  std::unordered_set<std::shared_ptr<Mesh>> visitedMeshes;

  for (auto &&drawable : stage->getDrawables2d()) {
    if (!std::holds_alternative<TextPtr>(drawable)) {
      continue;
    }

    auto text = std::get<TextPtr>(drawable);

    if (!text->getFont()) {
      continue;
    }

    auto future = text->getFont()->get();
    auto font = future.get();

    if (mLoadedFonts.count(font) == 0) {
      mLoadedFonts.insert(font);
    }
  }

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

std::forward_list<ImageResPtr> CommonRenderer::listImagesInUse(std::shared_ptr<Stage> stage) {
  std::forward_list<ImageResPtr> images;

  for (auto &&drawable : stage->getDrawables2d()) {
    std::visit(overloaded {
      [](auto) {},
      [&](SpritePtr sprite) {
        if (sprite->getImage()) {
          images.emplace_front(sprite->getImage());
        }
      }
    }, drawable);
  }

  // todo: font characters

  for (auto &&model : stage->getDrawables3d()) {
    auto diffuse = model->getMaterial().getDiffuse();
    auto normal = model->getMaterial().getNormal();

    if (diffuse) {
      images.emplace_front(diffuse);
    }

    if (normal) {
      images.emplace_front(normal);
    }
  }

  return images;
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

void CommonRenderer::render2d(Canvas *canvas, [[maybe_unused]] Vector2i renderSize) {
  for (auto &&drawable : canvas->getStage()->getDrawables2d()) {
    std::visit(overloaded {
      [](auto) {},
      [&](SpritePtr sprite) {
        RenderTextureInfo info;
        auto texture = mKnownImages.at(sprite->getImage());
        info.textureId = texture.id;
        info.size = texture.size;
        info.position = sprite->getPosition() - canvas->getCamera2d().getPosition();
        renderTexture(canvas, &info);
      },
      [&](TextPtr text) {
        auto font = text->getFont()->get().get();

        int x = 0;
        int y = font->getBaseLine();

        for (auto &&cp : text->getContent()) {
          if (cp == '\n') {
            x = 0;
            y += font->getLineHeight();
            continue;
          }

          auto ch = font->getCharByCodePoint(cp);

          if (!ch) {
            // unknown character
            continue;
          }

          RenderTextureInfo info;

          if (ch->image) {
            if (mCharImages.count(ch) == 0) {
              int textureId = mTextureIdAllocator.next();
              mCharImages.emplace(ch, textureId);
              createTexture(textureId);
              loadTexture(textureId, ch->image);
            }

            info.textureId = mCharImages.at(ch);
            info.size = ch->image->getSize();
            info.position = Vector2i(x, y) + ch->offset;
            renderTexture(canvas, &info);
          }

          x += ch->advance;
        }
      }
    }, drawable);
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

  if (getCanvasSize().x() > scaledSize.x()) {
    pos.x(static_cast<float>(getCanvasSize().x() - scaledSize.x()) / 2);
  } else {
    pos.y(static_cast<float>(getCanvasSize().y() - scaledSize.y()) / 2);
  }

  RenderTextureInfo info;
  info.textureId = mRenderTargetId;
  info.size = scaledSize;
  info.position = pos;
  renderTexture(canvas, &info);
}
