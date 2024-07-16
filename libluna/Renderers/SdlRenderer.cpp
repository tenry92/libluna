#include <libluna/config.h>

#ifdef LUNA_RENDERER_SDL2
#include <libluna/Renderers/SdlRenderer.hpp>

#include <list>
#include <map>

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

#include <libluna/CanvasImpl.hpp>

#define CHECK_SDL(x)                                                           \
  {                                                                            \
    if (x) {                                                                   \
      logError("sdl error: {}", SDL_GetError());                               \
      Application::getInstance()->raiseCriticalError("SDL Error");             \
    }                                                                          \
  }

using namespace Luna;

static bool gDidPrintRenderDrivers{false};

struct SdlDeleter {
  void operator()(SDL_Renderer *renderer) const {
    logVerbose("destroying SDL renderer");
    SDL_DestroyRenderer(renderer);
  }

  void operator()(SDL_Texture *texture) const {
    logDebug("destroying SDL texture");
    SDL_DestroyTexture(texture);
  }
};

class SdlRenderer::impl {
  public:
#ifdef LUNA_IMGUI
  ImGuiContext *mImGuiContext{nullptr};
#endif

  std::unique_ptr<SDL_Renderer, SdlDeleter> mRenderer;
  std::shared_ptr<Internal::GraphicsMetrics> mMetrics;

  std::map<int, SDL_Texture *> mTextureIdMapping;
};

SdlRenderer::SdlRenderer() : mImpl{std::make_unique<impl>()} {
  mImpl->mMetrics = std::make_shared<Internal::GraphicsMetrics>();
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
  std::unique_ptr<SDL_Renderer, SdlDeleter> renderer(SDL_CreateRenderer(
      getCanvas()->getImpl()->sdl.window,
      -1, /* -1 = most suitable driver, 0 = first available driver */
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  ));
  logDebug("sdl renderer created");

  if (!renderer) {
    logError("error creating sdl renderer: {}", SDL_GetError());
  }

  logDebug("move renderer");
  mImpl->mRenderer = std::move(renderer);
  logDebug("moved renderer");
}

void SdlRenderer::initializeImmediateGui() {
#ifdef LUNA_IMGUI
  IMGUI_CHECKVERSION();
  mImpl->mImGuiContext = ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(
      getCanvas()->getImpl()->sdl.window, mImpl->mRenderer.get()
  );
  ImGui_ImplSDLRenderer2_Init(mImpl->mRenderer.get());

  // todo(?): load fonts
#endif
}

void SdlRenderer::quitImmediateGui() {
#ifdef LUNA_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext(mImpl->mImGuiContext);
    mImpl->mImGuiContext = nullptr;
  }
#endif
}

void SdlRenderer::close() {
#ifdef LUNA_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplSDLRenderer2_Shutdown();

    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    mImpl->mImGuiContext = nullptr;
  }
#endif

  mImpl->mRenderer.reset();
}

void SdlRenderer::present() {
#ifdef LUNA_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
  }
#endif

  SDL_RenderPresent(mImpl->mRenderer.get());
}

Internal::GraphicsMetrics SdlRenderer::getMetrics() {
  return *mImpl->mMetrics;
}


void SdlRenderer::clearBackground(ColorRgb color) {
  auto color32 = makeColorRgb32(color);
  CHECK_SDL(SDL_SetRenderDrawColor(
      mImpl->mRenderer.get(), color32.red, color32.green, color32.blue, color32.alpha
  ));
  CHECK_SDL(SDL_RenderClear(mImpl->mRenderer.get()));
}

void SdlRenderer::createTexture([[maybe_unused]] int id) {
  // stub
}

void SdlRenderer::destroyTexture(int id) {
  SDL_Texture *texture = mImpl->mTextureIdMapping.at(id);
  mImpl->mTextureIdMapping.erase(id);
  SDL_DestroyTexture(texture);
}

void SdlRenderer::loadTexture(int id, ImagePtr image) {
  if (mImpl->mTextureIdMapping.count(id)) {
    SDL_Texture *oldTexture = mImpl->mTextureIdMapping.at(id);
    SDL_DestroyTexture(oldTexture);
    mImpl->mTextureIdMapping.erase(id);
  }

  uint32_t surfaceFormat = SDL_PIXELFORMAT_RGBA32;

  switch (image->getBitsPerPixel()) {
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

  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
      (void *)(image->getData()), image->getSize().x(),
      image->getSize().y(), image->getBitsPerPixel(), image->getBytesPerRow(),
      surfaceFormat
  );

  auto texture =
      SDL_CreateTextureFromSurface(mImpl->mRenderer.get(), surface);

  SDL_FreeSurface(surface);

  mImpl->mTextureIdMapping.emplace(id, texture);
}

void SdlRenderer::resizeTexture([[maybe_unused]] int id, [[maybe_unused]] Vector2i size) {
  if (mImpl->mTextureIdMapping.count(id)) {
    SDL_Texture *oldTexture = mImpl->mTextureIdMapping.at(id);
    mImpl->mTextureIdMapping.erase(id);
    SDL_DestroyTexture(oldTexture);
  }

  SDL_Texture *texture = SDL_CreateTexture(mImpl->mRenderer.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, size.x(), size.y());
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  mImpl->mTextureIdMapping.emplace(id, texture);
}

void SdlRenderer::renderTexture([[maybe_unused]] Canvas *canvas, RenderTextureInfo *info) {
  auto texture = mImpl->mTextureIdMapping.at(info->textureId);

  SDL_Rect dstrect = {
      static_cast<int>(info->position.x()), static_cast<int>(info->position.y()),
      info->size.x(), info->size.y()};
  CHECK_SDL(SDL_RenderCopy(
      mImpl->mRenderer.get(), texture, nullptr, &dstrect
  ));
}

void SdlRenderer::setTextureFilterEnabled([[maybe_unused]] int id, [[maybe_unused]] bool enabled) {

}

void SdlRenderer::setRenderTargetTexture(int id) {
  auto texture = mImpl->mTextureIdMapping.at(id);
  SDL_SetRenderTarget(mImpl->mRenderer.get(), texture);
}

void SdlRenderer::unsetRenderTargetTexture() {
  SDL_SetRenderTarget(mImpl->mRenderer.get(), nullptr);
}

void SdlRenderer::setViewport(Vector2i offset, Vector2i size) {
  SDL_Rect viewport;
  viewport.x = offset.x();
  viewport.y = offset.y();
  viewport.w = size.x();
  viewport.h = size.y();

  SDL_RenderSetViewport(mImpl->mRenderer.get(), &viewport);
}


void SdlRenderer::imguiNewFrame() {
#ifdef LUNA_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
  }
#endif
}

#endif
