#include <libluna/config.h>

#ifdef LUNA_RENDERER_SDL2
#include <libluna/Renderers/SdlRenderer.hpp>

#include <list>
#include <map>
#include <vector>

#ifdef LUNA_WINDOW_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#ifdef LUNA_IMGUI
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_sdlrenderer2.h>
#include <imgui/imgui.h>
#endif // IMGUI

#include <libluna/Application.hpp>
#include <libluna/Logger.hpp>
#include <libluna/MemoryReader.hpp>

#include <libluna/Canvas.hpp>

using namespace Luna;

#define CHECK_SDL(x)                                                           \
  {                                                                            \
    if (x) {                                                                   \
      logError("sdl error: {}", SDL_GetError());                               \
      Application::getInstance()->raiseCriticalError("SDL Error");             \
    }                                                                          \
  }

template<typename T>
inline T* assertSdl(T* ptr) {
  if (ptr == nullptr) {
    logError("sdl error: {}", SDL_GetError());
    Application::getInstance()->raiseCriticalError("SDL Error");
  }

  return ptr;
}

static bool gDidPrintRenderDrivers{false};

SdlRenderer::SdlRenderer() {
  mMetrics = std::make_shared<Internal::GraphicsMetrics>();
}

SdlRenderer::~SdlRenderer() = default;

void SdlRenderer::initialize() {
  if (!SDL_WasInit(SDL_INIT_VIDEO)) {
    logInfo("initializing SDL video");
    SDL_InitSubSystem(SDL_INIT_VIDEO);
  }

  if (!gDidPrintRenderDrivers) {
    gDidPrintRenderDrivers = true;

    int numDrivers = SDL_GetNumRenderDrivers();

    for (int i = 0; i < numDrivers; ++i) {
      SDL_RendererInfo info;
      int result = SDL_GetRenderDriverInfo(i, &info);

      if (result != 0) {
        logError(
          "cannot retrieve sdl render driver info #{}: {}", i, SDL_GetError()
        );
        continue;
      }

      logDebug("sdl render driver #{}: {}", i, info.name);
    }
  }

  logInfo("creating SDL renderer");
  std::unique_ptr<SDL_Renderer, SdlDeleter> renderer(assertSdl(SDL_CreateRenderer(
    getCanvas()->sdl.window,
    -1, /* -1 = most suitable driver, 0 = first available driver */
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  )));
  logDebug("sdl renderer created");

  if (!renderer) {
    logError("error creating sdl renderer: {}", SDL_GetError());
  }

  logDebug("move renderer");
  mRenderer = std::move(renderer);
  logDebug("moved renderer");
}

void SdlRenderer::initializeImmediateGui() {
#ifdef LUNA_IMGUI
  if (mImGuiContext != nullptr) {
    return;
  }

  IMGUI_CHECKVERSION();
  mImGuiContext = ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(getCanvas()->sdl.window, mRenderer.get());
  ImGui_ImplSDLRenderer2_Init(mRenderer.get());

  // todo(?): load fonts
#endif
}

void SdlRenderer::quitImmediateGui() {
#ifdef LUNA_IMGUI
  if (mImGuiContext) {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext(mImGuiContext);
    mImGuiContext = nullptr;
  }
#endif
}

void SdlRenderer::close() {
#ifdef LUNA_IMGUI
  if (mImGuiContext) {
    ImGui_ImplSDLRenderer2_Shutdown();

    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    mImGuiContext = nullptr;
  }
#endif

  mRenderer.reset();
}

void SdlRenderer::present() {
#ifdef LUNA_IMGUI
  if (mImGuiContext) {
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
  }
#endif

  SDL_RenderPresent(mRenderer.get());
}

Internal::GraphicsMetrics SdlRenderer::getMetrics() { return *mMetrics; }

void SdlRenderer::clearBackground(ColorRgb color) {
  auto color32 = makeColorRgb32(color);
  CHECK_SDL(SDL_SetRenderDrawColor(
    mRenderer.get(), color32.red, color32.green, color32.blue, color32.alpha
  ));
  CHECK_SDL(SDL_RenderClear(mRenderer.get()));
}

void SdlRenderer::createFramebufferTexture(uint16_t id, Vector2i size) {
  SDL_Texture* texture = assertSdl(SDL_CreateTexture(
    mRenderer.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
    size.width, size.height
  ));
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  mTextureIdMapping.emplace(id, texture);
}

void SdlRenderer::resizeFramebufferTexture(uint16_t id, Vector2i size) {
  if (mTextureIdMapping.count(id)) {
    SDL_Texture* oldTexture = mTextureIdMapping.at(id);
    mTextureIdMapping.erase(id);
    SDL_DestroyTexture(oldTexture);
  }

  SDL_Texture* texture = assertSdl(SDL_CreateTexture(
    mRenderer.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
    size.width, size.height
  ));
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  mTextureIdMapping.emplace(id, texture);
}

void SdlRenderer::destroyFramebufferTexture(uint16_t id) {
  SDL_Texture* texture = mTextureIdMapping.at(id);
  mTextureIdMapping.erase(id);
  SDL_DestroyTexture(texture);
}

void SdlRenderer::freeTexture(int slot) {
  auto gpuTexture = getGpuTexture(slot);

  if (gpuTexture) {
    if (gpuTexture->id != 0) {
      auto textureIt = mTextureIdMapping.find(gpuTexture->id);
      if (textureIt != mTextureIdMapping.end()) {
        SDL_DestroyTexture(textureIt->second);
        mTextureIdMapping.erase(textureIt);
      }
    }

    for (auto& subTexture : gpuTexture->subTextures) {
      auto textureIt = mTextureIdMapping.find(subTexture.id);
      if (textureIt != mTextureIdMapping.end()) {
        SDL_DestroyTexture(textureIt->second);
        mTextureIdMapping.erase(textureIt);
      }
    }
  }

  freeGpuTexture(slot);
}

void SdlRenderer::uploadTexture(int slot, const Texture* texture) {
  freeTexture(slot);

  GpuTexture gpuTexture;
  gpuTexture.size = texture->getSize();

  declareGpuTexture(slot, gpuTexture);

  uint32_t surfaceFormat = SDL_PIXELFORMAT_RGBA32;

  switch (texture->getBitsPerPixel()) {
  case 16:
    surfaceFormat = SDL_PIXELFORMAT_ABGR1555;
    break;
  case 24:
    surfaceFormat = SDL_PIXELFORMAT_RGB24;
    break;
  case 32:
    surfaceFormat = SDL_PIXELFORMAT_RGBA32;
    break;
  }

  SDL_Surface* surface = assertSdl(SDL_CreateRGBSurfaceWithFormatFrom(
    (void*)(texture->getData()), texture->getSize().width, texture->getSize().height,
    texture->getBitsPerPixel(), texture->getBytesPerRow(), surfaceFormat
  ));

  auto sdlTexture = assertSdl(SDL_CreateTextureFromSurface(mRenderer.get(), surface));

  SDL_FreeSurface(surface);

  mTextureIdMapping.emplace(gpuTexture.id, sdlTexture);
}

void SdlRenderer::renderTexture(
  [[maybe_unused]] Canvas* canvas, RenderTextureInfo* info
) {
  auto texture = mTextureIdMapping.at(info->textureId);

  SDL_Rect srcrect = {
    info->crop.x, info->crop.y, info->crop.width, info->crop.height};

  SDL_Rect dstrect = {
    static_cast<int>(info->position.x), static_cast<int>(info->position.y),
    info->size.width, info->size.height};
  if (srcrect.w == 0 || srcrect.h == 0) {
    CHECK_SDL(SDL_RenderCopy(mRenderer.get(), texture, nullptr, &dstrect));
  } else {
    CHECK_SDL(SDL_RenderCopy(mRenderer.get(), texture, &srcrect, &dstrect));
  }
}

void SdlRenderer::createShape([[maybe_unused]] int id) {}

void SdlRenderer::destroyShape([[maybe_unused]] int id) {
  mShapeIdMapping.erase(id);
}

void SdlRenderer::loadShape(
  [[maybe_unused]] int id, [[maybe_unused]] Shape* shape
) {
  mShapeIdMapping.emplace(id, shape);
}

void SdlRenderer::renderShape(
  [[maybe_unused]] Canvas* canvas, [[maybe_unused]] RenderShapeInfo* info
) {
  auto shape = mShapeIdMapping.at(info->shapeId);

  SDL_SetRenderDrawColor(mRenderer.get(), 255, 0, 0, 255);

  std::vector<SDL_FPoint> points;
  points.reserve(shape->getVertices().size());

  for (auto& point : shape->getVertices()) {
    points.push_back({info->position.x + point.x, info->position.y + point.y});
  }

  SDL_RenderDrawLinesF(
    mRenderer.get(), points.data(), static_cast<int>(points.size())
  );
}

void SdlRenderer::setTextureFilterEnabled(
  [[maybe_unused]] uint16_t id, [[maybe_unused]] bool enabled
) {}

void SdlRenderer::setRenderTargetTexture(uint16_t id) {
  auto texture = mTextureIdMapping.at(id);
  SDL_SetRenderTarget(mRenderer.get(), texture);
}

void SdlRenderer::unsetRenderTargetTexture() {
  SDL_SetRenderTarget(mRenderer.get(), nullptr);
}

void SdlRenderer::setViewport(Vector2i offset, Vector2i size) {
  SDL_Rect viewport;
  viewport.x = offset.x;
  viewport.y = offset.y;
  viewport.w = size.width;
  viewport.h = size.height;

  SDL_RenderSetViewport(mRenderer.get(), &viewport);
}

void SdlRenderer::imguiNewFrame() {
#ifdef LUNA_IMGUI
  if (mImGuiContext) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
  }
#endif
}

#endif
