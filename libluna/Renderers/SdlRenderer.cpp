#include <libluna/config.h>

#include <cmath>
#include <memory>

#ifdef LUNA_USE_SDL
#include <libluna/Renderers/SdlRenderer.hpp>

#include <list>
#include <map>

#include <SDL2/SDL.h>

#ifdef LUNA_USE_IMGUI
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_sdlrenderer2.h>
#include <imgui/imgui.h>
#endif // IMGUI

#include <libluna/Application.hpp>
#include <libluna/Image.hpp>
#include <libluna/Logger.hpp>
#include <libluna/ResourceRef.hpp>
#include <libluna/TextureCache.hpp>

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

class SdlTextureWrapper {
  public:
  SdlTextureWrapper(
      SDL_Texture *texture, int width, int height,
      std::shared_ptr<Internal::GraphicsMetrics> metrics
  )
      : mTexture{texture}, mWidth{width}, mHeight{height}, mMetrics{metrics} {
    ++mMetrics->textureCount;
  }

  ~SdlTextureWrapper() {
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
    --mMetrics->textureCount;
  }

  SDL_Texture *getTexture() const { return mTexture; }

  int getWidth() const { return mWidth; }

  int getHeight() const { return mHeight; }

  private:
  SDL_Texture *mTexture;
  int mWidth;
  int mHeight;
  std::shared_ptr<Internal::GraphicsMetrics> mMetrics;
};

using TextureType = std::shared_ptr<SdlTextureWrapper>;
using TextureCacheType = TextureCache<TextureType>;

class SdlRenderer::impl {
  public:
  std::unique_ptr<SDL_Renderer, SdlDeleter> mRenderer;
  std::unique_ptr<TextureCacheType> mTextureCache;
  std::shared_ptr<Internal::GraphicsMetrics> mMetrics;
#ifdef LUNA_USE_IMGUI
  ImGuiContext *mImGuiContext{nullptr};
#endif
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
#ifdef LUNA_USE_IMGUI
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
#ifdef LUNA_USE_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext(mImpl->mImGuiContext);
    mImpl->mImGuiContext = nullptr;
  }
#endif
}

void SdlRenderer::close() {
#ifdef LUNA_USE_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplSDLRenderer2_Shutdown();

    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    mImpl->mImGuiContext = nullptr;
  }
#endif

  mImpl->mRenderer.reset();
}

void SdlRenderer::render() {
#ifdef LUNA_USE_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
  }
#endif

  auto canvas = getCanvas();

  if (!canvas) {
    CHECK_SDL(SDL_SetRenderDrawColor(
        mImpl->mRenderer.get(), 0, 0, 0, SDL_ALPHA_OPAQUE
    ));
    CHECK_SDL(SDL_RenderClear(mImpl->mRenderer.get()));

    return;
  }

  auto bgColor = canvas->getBackgroundColor();
  CHECK_SDL(SDL_SetRenderDrawColor(
      mImpl->mRenderer.get(), static_cast<uint8_t>(bgColor.reduceRed(8)),
      static_cast<uint8_t>(bgColor.reduceGreen(8)),
      static_cast<uint8_t>(bgColor.reduceBlue(8)), SDL_ALPHA_OPAQUE
  ));
  CHECK_SDL(SDL_RenderClear(mImpl->mRenderer.get()));

  if (!canvas->getStage()) {
    return;
  }

  if (!mImpl->mTextureCache ||
      mImpl->mTextureCache->getStage() != canvas->getStage()) {
    std::function<TextureType(std::shared_ptr<Image>, int)> callback =
        [this](std::shared_ptr<Image> image, int frameIndex) -> TextureType {
      logDebug("loading sdl texture from image#{}", frameIndex);

      if (!image->isTrue()) {
        /// @todo Get palette from sprite
        image = std::make_shared<Image>(image->toTrue(nullptr));
      }

      SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
          (void *)(image->getFrameData(frameIndex)), image->getSize().x(),
          image->getSize().y(), 32, image->getBytesPerLine(),
          SDL_PIXELFORMAT_RGBA32
      );

      auto texture =
          SDL_CreateTextureFromSurface(mImpl->mRenderer.get(), surface);

      SDL_FreeSurface(surface);

      return std::make_shared<SdlTextureWrapper>(
          texture, image->getSize().x(), image->getSize().y(), mImpl->mMetrics
      );
    };
    mImpl->mTextureCache =
        std::make_unique<TextureCacheType>(canvas->getStage(), callback);
  }

  mImpl->mTextureCache->updateCache();

  // CHECK_SDL(SDL_SetRenderDrawColor(mImpl->mRenderer.get(), 111, 0, 111,
  // SDL_ALPHA_OPAQUE)); SDL_Rect f = {0, 0, 1920, 1080};
  // CHECK_SDL(SDL_RenderFillRect(mImpl->mRenderer.get(), &f));

  auto camera2d = canvas->getCamera2d();

  // drawDebugGrid();

  for (auto &&sprite : canvas->getStage()->getSprites()) {
    if (!mImpl->mTextureCache->hasTextureBySprite(sprite)) {
      continue;
    }

    auto texture = mImpl->mTextureCache->getTextureBySprite(sprite);
    auto position = sprite->getPosition() - camera2d.getPosition();

    SDL_Rect dstrect = {
        static_cast<int>(position.x()), static_cast<int>(position.y()),
        texture->getWidth(), texture->getHeight()};
    CHECK_SDL(SDL_RenderCopy(
        mImpl->mRenderer.get(), texture->getTexture(), nullptr, &dstrect
    ));
  }
}

void SdlRenderer::present() {
#ifdef LUNA_USE_IMGUI
  if (mImpl->mImGuiContext) {
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
  }
#endif

  SDL_RenderPresent(mImpl->mRenderer.get());
}

void SdlRenderer::drawDebugGrid() {
  auto canvas = getCanvas();
  auto camera2d = canvas->getCamera2d();
  int screenWidth, screenHeight;
  SDL_GetWindowSize(
      getCanvas()->getImpl()->sdl.window, &screenWidth, &screenHeight
  );

  for (int x = 0; x < screenWidth + 15; x += 16) {
    auto camX = camera2d.getPosition().x();
    auto offset = static_cast<int>(-std::fmod(camX, 32));
    auto isMain = x % 32 == 0;
    auto value = static_cast<uint8_t>(isMain ? 127 : 63);
    CHECK_SDL(SDL_SetRenderDrawColor(
        mImpl->mRenderer.get(), value, value, value, isMain ? 127 : 63
    ));
    CHECK_SDL(SDL_RenderDrawLine(
        mImpl->mRenderer.get(), x + offset, 0, x + offset, screenHeight
    ));
  }

  for (int y = 0; y < screenHeight + 15; y += 16) {
    auto camY = camera2d.getPosition().y();
    auto offset = static_cast<int>(-std::fmod(camY, 32));
    auto isMain = y % 32 == 0;
    auto value = static_cast<uint8_t>(isMain ? 127 : 63);
    CHECK_SDL(SDL_SetRenderDrawColor(
        mImpl->mRenderer.get(), value, value, value, isMain ? 127 : 63
    ));
    CHECK_SDL(SDL_RenderDrawLine(
        mImpl->mRenderer.get(), 0, y + offset, screenWidth, y + offset
    ));
  }

  CHECK_SDL(SDL_SetRenderDrawColor(mImpl->mRenderer.get(), 191, 0, 0, 127));
  for (int i = -2; i <= 2; ++i) {
    CHECK_SDL(SDL_RenderDrawLine(
        mImpl->mRenderer.get(), 0,
        static_cast<int>(-camera2d.getPosition().y()) + i, screenWidth,
        static_cast<int>(-camera2d.getPosition().y()) + i
    ));
  }

  CHECK_SDL(SDL_SetRenderDrawColor(mImpl->mRenderer.get(), 0, 127, 0, 127));
  for (int i = -2; i <= 2; ++i) {
    CHECK_SDL(SDL_RenderDrawLine(
        mImpl->mRenderer.get(),
        static_cast<int>(-camera2d.getPosition().x()) + i, 0,
        static_cast<int>(-camera2d.getPosition().x()) + i, screenHeight
    ));
  }
}

Internal::GraphicsMetrics SdlRenderer::getMetrics() { return *mImpl->mMetrics; }

#endif
