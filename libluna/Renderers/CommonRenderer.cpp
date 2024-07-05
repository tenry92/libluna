#include <libluna/config.h>

#include <unordered_set>

#ifdef LUNA_USE_SDL
#include <SDL2/SDL.h>
#endif

#ifdef LUNA_USE_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#ifdef LUNA_USE_EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#ifdef __SWITCH__
#include <switch.h>
#endif

#include <libluna/Renderers/CommonRenderer.hpp>
#include <libluna/CanvasImpl.hpp>
#include <libluna/Logger.hpp>

using namespace Luna;

CommonRenderer::CommonRenderer() : mRenderTargetId{0} {}

CommonRenderer::~CommonRenderer() {
  // todo
}

void CommonRenderer::render() {
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

  if (!mRenderTargetId) {
    mRenderTargetId = mTextureIdAllocator.next();
    logDebug("create texture #{} (render target)", mRenderTargetId);
    createTexture(mRenderTargetId);
  }

  // render sprites to internal texture
  start2dFramebuffer(canvas);
  renderSprites(canvas, getInternalSize());
  end2dFramebuffer(canvas);
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

#ifdef LUNA_USE_SDL
  SDL_GetWindowSize(
      getCanvas()->getImpl()->sdl.window, &canvasWidth, &canvasHeight
  );
#endif
#ifdef LUNA_USE_GLFW
  glfwGetFramebufferSize(
      getCanvas()->getImpl()->glfw.window, &canvasWidth, &canvasHeight
  );
#endif
#ifdef __SWITCH__
  ViDisplay display;
  viOpenDefaultDisplay(&display);
  viGetDisplayResolution(&display, &canvasWidth, &canvasHeight);
#endif

  return Vector2i(canvasWidth, canvasHeight);
}

Vector2i CommonRenderer::getInternalSize() const {
  return getCanvas()->getImpl()->mOriginalSize;
}

Vector2i CommonRenderer::getCurrentRenderSize() const {
  return mCurrentRenderSize;
}

void CommonRenderer::updateTextureCache(std::shared_ptr<Stage> stage) {
  std::unordered_set<ImageResPtr> visitedImages;
  std::unordered_set<std::shared_ptr<Mesh>> visitedMeshes;
  
  for (auto &&sprite : stage->getSprites()) {
    if (!sprite->getImage()) {
      continue;
    }

    visitedImages.emplace(sprite->getImage());

    if (mKnownImages.count(sprite->getImage()) == 0) {
      auto future = sprite->getImage()->get();
      // check with each update whether future is ready:
      // if (imageRef->isReady()):
      auto image = future.get();
      int textureId = mTextureIdAllocator.next();
      mKnownImages.emplace(sprite->getImage(), textureId);
      mImageSizes.emplace(sprite->getImage(), image->getSize());
      logDebug("create texture #{} (sprite)", textureId);
      createTexture(textureId);
      loadTexture(textureId, image);
    }
  }

  for (auto &&model : stage->getModels()) {
    visitedMeshes.emplace(model->getMesh());

    if (mKnownMeshes.count(model->getMesh()) == 0) {
      int meshId = mMeshIdAllocator.next();
      mKnownMeshes.emplace(model->getMesh(), meshId);
      createMesh(meshId);
      loadMesh(meshId, model->getMesh());
    }

    auto diffuse = model->getMaterial().getDiffuse();
    auto normal = model->getMaterial().getNormal();

    if (diffuse) {
      visitedImages.emplace(diffuse);

      if (mKnownImages.count(diffuse) == 0) {
        int textureId = mTextureIdAllocator.next();
        mKnownImages.emplace(diffuse, textureId);
        logDebug("create texture #{} (diffuse)", textureId);
        createTexture(textureId);

        auto future = diffuse->get();
        // check with each update whether future is ready:
        // if (imageRef->isReady()):
        auto image = future.get();
        loadTexture(textureId, image);
        mImageSizes.emplace(diffuse, image->getSize());
      }
    }

    if (normal) {
      visitedImages.emplace(normal);

      if (mKnownImages.count(normal) == 0) {
        int textureId = mTextureIdAllocator.next();
        mKnownImages.emplace(normal, textureId);
        logDebug("create texture #{} (normal)", textureId);
        createTexture(textureId);

        auto future = normal->get();
        // check with each update whether future is ready:
        // if (imageRef->isReady()):
        auto image = future.get();
        loadTexture(textureId, image);
        mImageSizes.emplace(normal, image->getSize());
      }
    }
  }

  for (auto it = mKnownImages.begin(); it != mKnownImages.end();) {
    if (visitedImages.find(it->first) == visitedImages.end()) {
      // todo: delete all frames of an animated image
      logDebug("destroy texture #{}", it->second);
      mTextureIdAllocator.free(static_cast<uint8_t>(it->second));
      destroyTexture(it->second);
      mImageSizes.erase(it->first);
      it = mKnownImages.erase(it);
    } else {
      ++it;
    }
  }
}

void CommonRenderer::renderWorld(Canvas *canvas) {
  for (auto &&model : canvas->getStage()->getModels()) {
    RenderMeshInfo info;
    info.meshId = mKnownMeshes.at(model->getMesh());
    info.transform = model->getTransform();

    auto material = model->getMaterial();

    if (material.getDiffuse()) {
      info.diffuseTextureId = mKnownImages.at(material.getDiffuse());
    }

    if (material.getNormal()) {
      info.normalTextureId = mKnownImages.at(material.getNormal());
    }

    renderMesh(canvas, &info);
  }
}

void CommonRenderer::renderSprites(Canvas *canvas, [[maybe_unused]] Vector2i renderSize) {
  for (auto &&sprite : canvas->getStage()->getSprites()) {
    RenderTextureInfo info;
    info.textureId = mKnownImages.at(sprite->getImage());
    info.size = mImageSizes.at(sprite->getImage());
    info.position = sprite->getPosition() - canvas->getCamera2d().getPosition();
    renderTexture(canvas, &info);
  }
}

void CommonRenderer::start2dFramebuffer([[maybe_unused]] Canvas *canvas) {
  resizeTexture(mRenderTargetId, getInternalSize());
  setRenderTargetTexture(mRenderTargetId);
  setViewport({0, 0}, getInternalSize());
  mCurrentRenderSize = getInternalSize();
  clearBackground(ColorRgb{0, 0, 0, 0});
}

void CommonRenderer::end2dFramebuffer(Canvas *canvas) {
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
