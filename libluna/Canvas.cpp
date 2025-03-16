#include <libluna/config.h>

#ifdef LUNA_WINDOW_EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <glad/glad.h>
#endif

#ifdef __SWITCH__
#include <switch.h>
#endif

#include <libluna/Canvas.hpp>

#include <functional>

#include <fmt/format.h>

#include <libluna/AbstractRenderer.hpp>
#include <libluna/Application.hpp>
#include <libluna/Console.hpp>
#include <libluna/Logger.hpp>

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#ifdef LUNA_RENDERER_SDL2
#include <libluna/Renderers/SdlRenderer.hpp>
#endif
#endif

#ifdef LUNA_RENDERER_OPENGL
#include <libluna/Renderers/OpenglRenderer.hpp>
#endif

#ifdef NDS
#include <libluna/Renderers/NdsRenderer.hpp>
#endif

#ifdef N64
#include <libdragon.h>
#include <libluna/Renderers/N64Renderer.hpp>
#endif

using namespace Luna;
using namespace Luna::Internal;
using Luna::String;

namespace {
  static void printDisplays() {
    static bool didPrint = false;

    if (didPrint) {
      return;
    }

    didPrint = true;

#ifdef LUNA_WINDOW_SDL2
    int displayCount = SDL_GetNumVideoDisplays();

    for (int i = 0; i < displayCount; ++i) {
      SDL_DisplayMode mode;

      if (SDL_GetCurrentDisplayMode(i, &mode) == 0) {
        Console::writeLine(
          "Display {} ({}): {}x{} @ {}Hz", i, SDL_GetDisplayName(i), mode.w, mode.h, mode.refresh_rate
        );
      }
    }
#endif
  }

  std::map<Stage*, std::list<Canvas*>> gCanvasByStage;

#ifdef LUNA_RENDERER_OPENGL
  [[maybe_unused]] void setGlAttributes() {
#ifdef LUNA_WINDOW_SDL2
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // #ifndef NDEBUG
#endif // #ifdef LUNA_WINDOW_SDL2

#ifdef LUNA_USE_GLFW
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 0);
#endif // #ifdef LUNA_USE_GLFW
  }
#endif // #ifdef LUNA_RENDERER_OPENGL
} // namespace

Command::~Command() = default;

class CanvasCommand : public Command {
  public:
  CanvasCommand(std::function<void()> callback) : mCallback(callback) {}

  void execute() const override { mCallback(); }

  private:
  std::function<void()> mCallback;
};

void Canvas::createWindow(const DisplayMode& displayMode) {
  Console::quit();

#ifdef LUNA_WINDOW_SDL2
  if (!SDL_WasInit(SDL_INIT_VIDEO)) {
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    printDisplays();
  }

  if (this->sdl.window) {
    SDL_DestroyWindow(this->sdl.window);
    this->sdl.window = nullptr;
  }

  logInfo("creating SDL window ({}x{} pixels)", displayMode.resolution.width, displayMode.resolution.height);

  Uint32 windowFlags = SDL_WINDOW_RESIZABLE;

  if (displayMode.fullscreen) {
    // note: SDL_WINDOW_FULLSCREEN may change the display resolution
    windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

#ifdef LUNA_RENDERER_OPENGL
  if (displayMode.videoDriver == "opengl") {
    setGlAttributes();
    windowFlags |= SDL_WINDOW_OPENGL;
  }
#endif

  this->sdl.window = SDL_CreateWindow(
    fmt::format("{} ({})", Application::getInstance()->getName().c_str(), displayMode.videoDriver.c_str())
      .c_str(),
#if SDL_MAJOR_VERSION == 2
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
#endif
    displayMode.resolution.width, displayMode.resolution.height, windowFlags
  );

  if (!this->sdl.window) {
    logError("error creating sdl window: {}", SDL_GetError());
  }
#endif

#ifdef LUNA_WINDOW_EGL
  this->egl.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglInitialize(this->egl.display, nullptr, nullptr);
  eglBindAPI(EGL_OPENGL_API);

  EGLConfig config;
  EGLint numConfigs;
  static const EGLint framebufferAttributeList[] = {
    EGL_RENDERABLE_TYPE,
    EGL_OPENGL_BIT,
    EGL_RED_SIZE,
    8,
    EGL_GREEN_SIZE,
    8,
    EGL_BLUE_SIZE,
    8,
    EGL_ALPHA_SIZE,
    8,
    EGL_DEPTH_SIZE,
    24,
    EGL_STENCIL_SIZE,
    8,
    EGL_NONE};
  eglChooseConfig(
    this->egl.display, framebufferAttributeList, &config, 1, &numConfigs
  );

  this->egl.surface = eglCreateWindowSurface(
    this->egl.display, config, nwindowGetDefault(), nullptr
  );

  static const EGLint contextAttributeList[] = {
    EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,
    EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
    EGL_CONTEXT_MAJOR_VERSION_KHR,
    4,
    EGL_CONTEXT_MINOR_VERSION_KHR,
    3,
    EGL_NONE};

  this->egl.context = eglCreateContext(
    this->egl.display, config, EGL_NO_CONTEXT, contextAttributeList
  );

  eglMakeCurrent(
    this->egl.display, this->egl.surface, this->egl.surface, this->egl.context
  );
#endif
}

void Canvas::processCommandQueue() {
#ifdef LUNA_THREADED_CANVAS
  mCv.notify_one();
#else
  while (!mCommandQueue.empty()) {
    auto command = mCommandQueue.front();
    mCommandQueue.pop();
    command->execute();
  }
#endif
}

Internal::GraphicsMetrics Canvas::getMetrics() {
  if (!mRenderer) {
    return {};
  }

  return mRenderer->getMetrics();
}

#ifdef LUNA_THREADED_CANVAS
static int gNextThreadId = 0;

void Canvas::renderThread() {
  int threadId = ++gNextThreadId;
  Logger::getInstance().setThreadIdentifier(fmt::format("Render {}", threadId));
  logDebug("renderThread #{} spawned", threadId);

  std::unique_lock lock(mMutex);

  logDebug("entering renderThread #{} loop", threadId);

  while (true) {
    mCv.wait(lock, [this]() {
      return mExitRequested || !mCommandQueue.empty();
    });

    if (mExitRequested) {
      logDebug("exiting renderThread #{}", threadId);

      if (mRenderer) {
        mRenderer->close();
        mRenderer.reset();
      }

      mExitRequested = false;
      return;
    }

    while (!mCommandQueue.empty()) {
      auto command = mCommandQueue.front();
      mCommandQueue.pop();
      command->execute();
    }

    mCv.notify_one();
  }
}
#endif

#ifdef LUNA_WINDOW_SDL2
bool Canvas::sdlEventTargetsThis(const SDL_Event* event) {
  auto myWindowId = SDL_GetWindowID(this->sdl.window);

  switch (event->type) {
  case SDL_MOUSEMOTION:
    return event->motion.windowID == myWindowId;
  case SDL_MOUSEWHEEL:
    return event->wheel.windowID == myWindowId;
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    return event->button.windowID == myWindowId;
  case SDL_KEYDOWN:
  case SDL_KEYUP:
    return event->key.windowID == myWindowId;
  case SDL_TEXTINPUT:
    return event->text.windowID == myWindowId;
  case SDL_WINDOWEVENT:
    return event->window.windowID == myWindowId;
  }

  return true;
}

bool Canvas::sendSdlEventToImmediateGui(const SDL_Event* event) {
  if (mImmediateGuis.size() > 0) {
    bool result{false};
    auto command = std::make_shared<CanvasCommand>(([this, &result, event]() {
      result = ImmediateGui::processSdlEvent(event);
    }));

    mCommandQueue.emplace(command);
    processCommandQueue();
    sync();

    if (result) {
      return true;
    }
  }

  return false;
}
#endif

Canvas::Canvas(const Vector2i& size) : mSize{size}, mInternalResolution{size} {
#ifdef LUNA_THREADED_CANVAS
  mThread = std::thread(&Canvas::renderThread, this);
  this->sync();
#endif
}

Canvas::~Canvas() { this->close(); }

void Canvas::close() {
  setStage(nullptr);

  mImmediateGuis.clear();

#ifdef LUNA_THREADED_CANVAS
  if (mThread.joinable()) {
    {
      std::lock_guard lock(mMutex);
      mExitRequested = true;
    }
    mCv.notify_one();
    mThread.join();
  }
#else
  if (mRenderer) {
    mRenderer->close();
    mRenderer.reset();
  }
#endif

#ifdef LUNA_WINDOW_SDL2
  if (sdl.window) {
    logInfo("destorying SDL window");

#ifdef LUNA_RENDERER_OPENGL
    if (sdl.glContext) {
      SDL_GL_DeleteContext(sdl.glContext);
      sdl.glContext = nullptr;
    }
#endif
    SDL_DestroyWindow(sdl.window);
    sdl.window = nullptr;
  }
#endif

#ifdef LUNA_USE_GLFW
  if (glfw.window) {
    glfwDestroyWindow(glfw.window);
    glfw.window = nullptr;
  }
#endif

#ifdef LUNA_WINDOW_EGL
  if (egl.display) {
    eglMakeCurrent(egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    if (egl.context) {
      eglDestroyContext(egl.display, egl.context);
      egl.context = nullptr;
    }

    if (egl.surface) {
      eglDestroySurface(egl.display, egl.surface);
      egl.surface = nullptr;
    }

    // eglTerminate(egl.display);
    // egl.display = nullptr;
  }
#endif

  mClosed = true;
}

void Canvas::setDisplayMode(DisplayMode mode) {
  if (mode.videoDriver.isEmpty()) {
    mode.videoDriver = Application::getInstance()->getDefaultVideoDriver();
  }

  auto command = std::make_shared<CanvasCommand>(([this, mode]() {
    createWindow(mode);

    logInfo("setting canvas video driver to {}", mode.videoDriver.c_str());
#ifdef NDS
    mRenderer = std::make_unique<NdsRenderer>();
#endif
#ifdef N64
    resolution_t res = RESOLUTION_320x240;

    if (mode.resolution == Vector2i(256, 240)) {
      res = RESOLUTION_256x240;
    } else if (mode.resolution == Vector2i(320, 240)) {
      res = RESOLUTION_320x240;
    } else if (mode.resolution == Vector2i(512, 240)) {
      res = RESOLUTION_512x240;
    } else if (mode.resolution == Vector2i(640, 240)) {
      res = RESOLUTION_640x240;
    } else if (mode.resolution == Vector2i(512, 480)) {
      res = RESOLUTION_512x480;
    } else if (mode.resolution == Vector2i(640, 480)) {
      res = RESOLUTION_640x480;
    }

    display_init(
      res, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER
    );
    rdpq_init();
    mRenderer = std::make_unique<N64Renderer>();
#endif

#ifdef LUNA_WINDOW_SDL2
#ifdef LUNA_RENDERER_OPENGL
    if (mode.videoDriver == "opengl") {
      this->sdl.glContext = SDL_GL_CreateContext(this->sdl.window);
      SDL_GL_MakeCurrent(this->sdl.window, this->sdl.glContext);
      SDL_GL_SetSwapInterval(1); // enable vsync
      mRenderer = std::make_unique<OpenglRenderer>();
    }
#endif

    if (mode.videoDriver == "sdl") {
      mRenderer = std::make_unique<SdlRenderer>();
    }
#endif
#ifdef LUNA_USE_GLFW
    setGlAttributes();

    this->glfw.window = glfwCreateWindow(
      mode.resolution.width, mode.resolution.height,
      String("{} ({})")
        .format(Application::getInstance()->getName(), mode.videoDriver)
        .c_str(),
      nullptr, nullptr
    );

    if (!this->glfw.window) {
      logError("error creating glfw window");
    }

    if (mode.videoDriver == "opengl") {
      glfwMakeContextCurrent(this->glfw.window);
      glfwSwapInterval(1);
      mRenderer = std::make_unique<OpenglGraphicsDriver>();
    }
#endif
#ifdef LUNA_WINDOW_EGL
    mRenderer = std::make_unique<OpenglRenderer>();
#endif

    if (!mRenderer) {
      return;
    }

    mRenderer->setCanvas(this);

    mRenderer->initialize();
  }));

  mCommandQueue.emplace(command);

  processCommandQueue();
}

void Canvas::attachImmediateGui(std::unique_ptr<ImmediateGui> gui) {
  auto guiPtr = gui.get();
  mImmediateGuis.emplace_back(std::move(gui));

  auto command = std::make_shared<CanvasCommand>(([this, guiPtr]() {
    guiPtr->init(this);
    mRenderer->initializeImmediateGui();
  }));

  mCommandQueue.emplace(command);
  processCommandQueue();
}

void Canvas::detachImmediateGui(ImmediateGui* gui) {
  mImmediateGuis.remove_if([gui](auto& item) { return item.get() == gui; });

  if (mImmediateGuis.size() == 0) {
    auto command = std::make_shared<CanvasCommand>(([this]() {
      mRenderer->quitImmediateGui();
    }));

    mCommandQueue.emplace(command);
    processCommandQueue();
    this->sync();
  }
}

std::list<ImmediateGui*> Canvas::getImmediateGuis() const {
  std::list<ImmediateGui*> result;

  for (auto& gui : mImmediateGuis) {
    result.push_back(gui.get());
  }

  return result;
}

void Canvas::setStage(Stage* stage) {
  if (stage == mStage) {
    return;
  }

  mStage = stage;

  if (stage) {
    gCanvasByStage[stage].push_back(this);
  } else {
    gCanvasByStage[stage].remove(this);

    if (gCanvasByStage[stage].empty()) {
      gCanvasByStage.erase(stage);
    }
  }
}

Stage* Canvas::getStage() const { return mStage; }

void Canvas::setCamera2d(const Camera2d& camera) { mCamera2d = camera; }

Camera2d Canvas::getCamera2d() const { return mCamera2d; }

void Canvas::setCamera3d(const Camera3d& camera) { mCamera3d = camera; }

Camera3d Canvas::getCamera3d() const { return mCamera3d; }

void Canvas::setBackgroundColor(ColorRgb color) { mBackgroundColor = color; }

ColorRgb Canvas::getBackgroundColor() const { return mBackgroundColor; }

#ifdef LUNA_WINDOW_SDL2
ImagePtr Canvas::captureScreenshot() {
  SDL_Surface* surface = SDL_GetWindowSurface(sdl.window);
  // returns nullptr if SDL_FRAMEBUFFER_ACCELERATION=0 env is not set

  std::cout << "surface size: " << surface->w << "x" << surface->h << std::endl;

  std::cout << "first pixel: " << ((uint32_t*)surface->pixels)[0] << std::endl;

  // SDL renderer: SDL_RenderReadPixels(renderer, ...)

  return nullptr;
}
#endif

void Canvas::render() {
  if (!mRenderer) {
    return;
  }

  if (mStage) {
    mStage->updateTextureCache();
  }

  auto command = std::make_shared<CanvasCommand>(([this]() {
#if defined(LUNA_WINDOW_SDL2) && defined(LUNA_RENDERER_OPENGL)
    if (sdl.glContext) {
      SDL_GL_MakeCurrent(sdl.window, sdl.glContext);
    }
#endif

    mRenderer->render();

    for (auto& gui : mImmediateGuis) {
      gui->render(gui.get());
    }

    mRenderer->present();
  }));

  mCommandQueue.emplace(command);
  processCommandQueue();
}

void Canvas::sync() {
#ifdef LUNA_THREADED_CANVAS
  using namespace std::chrono_literals;

  std::unique_lock lock(mMutex);

#ifdef LUNA_WINDOW_SDL2
  // on Linux, SDL events are sent to the main thread only
  // on Windows, SDL events are sent to the individual threads
  auto command = std::make_shared<CanvasCommand>(([]() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      Application::getInstance()->pushSdlEvent(&event);
    }
  }));

  mCommandQueue.emplace(command);

  processCommandQueue();
#endif

  if (!mCommandQueue.empty()) {
    bool result =
      mCv.wait_for(lock, 4s, [this]() { return mCommandQueue.empty(); });

    if (!result) {
      logError("wait timeout expired and predicate still false");
    }
  }
#endif
}

bool Canvas::isClosed() const { return mClosed; }

void Canvas::setInternalResolution(Vector2i size) { mInternalResolution = size; }

Vector2i Canvas::getInternalResolution() const { return mInternalResolution; }

const std::list<Canvas*> Canvas::getCanvasByStage(Stage* stage) {
  return gCanvasByStage[stage];
}
