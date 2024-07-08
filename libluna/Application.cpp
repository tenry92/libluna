#include <libluna/config.h>

#include <libluna/Application.hpp>
#include <libluna/ApplicationImpl.hpp>

#include <string.h>

#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

#ifdef __SWITCH__
#include <switch.h>
#endif

#ifdef N64
#include <libdragon.h>
#endif

#ifdef LUNA_USE_SDL
#include <SDL2/SDL.h>
#endif

#ifdef LUNA_USE_GLFW
#include <GLFW/glfw3.h>
#endif

#ifdef LUNA_USE_IMGUI
#include <imgui/imgui.h>
#include <libluna/Internal/DebugGui.hpp>
#endif

#include <libluna/AudioManager.hpp>
#include <libluna/ImmediateGui.hpp>
#include <libluna/InputManager.hpp>
#include <libluna/Internal/DebugMetrics.hpp>
#include <libluna/IntervalManager.hpp>
#include <libluna/Logger.hpp>
#include <libluna/PathManager.hpp>
#include <libluna/Performance/Ticker.hpp>
#include <libluna/Platform.hpp>
#include <libluna/Terminal.hpp>

#include <libluna/CanvasImpl.hpp>

using std::cout, std::endl;

using namespace Luna;

static Application *gSingletonApp;

static void initSystem() {
  Terminal::init();

#ifdef __SWITCH__
  logInfo("initializing romfs");

  Result rc = romfsInit();

  if (R_FAILED(rc)) {
    logWarn("romfsInit failed: {}", rc);
  }
#endif

#ifdef LUNA_USE_SDL
  logInfo("initializing SDL");

  auto result = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_AUDIO);

  if (result != 0) {
    logError(SDL_GetError());
  }
#endif

#ifdef LUNA_USE_GLFW
  logInfo("initializing GLFW");
  if (!glfwInit()) {
    logError("unable to initialize GLFW");
  }
#endif
}

static void printCompiler() {
  for (auto &&info : Platform::getCompilerInfo()) {
#ifdef N64
    printf("%s\n", info.c_str());
#else
    cout << info.s_str() << endl;
#endif
  }
}

static void printDefines() {
#ifdef N64
  if (Platform::isDebug()) {
    printf("debug build (NDBEUG undefined)\n");
  } else {
    printf("release build (NDEBUG defined)\n");
  }

  printf("System: %s\n", Platform::getSystemName().c_str());
#else
  if (Platform::isDebug()) {
    cout << "debug build (NDBEUG undefined)" << endl;
  } else {
    cout << "release build (NDEBUG defined)" << endl;
  }

  cout << "System: " << Platform::getSystemName().s_str() << endl;
#endif
}

static void printArguments(const std::vector<String> &args) {
  for (std::size_t i = 0; i < args.size(); ++i) {
    cout << "Argument " << i << ": " << args[i].s_str() << endl;
  }
}

ApplicationImpl::ApplicationImpl(Application *app) {
  gSingletonApp = app;

  mHotkeysManager.setReturnUnused(true);
  mHotkeysManager.addButtonBinding("Toggle Debugger", "Keyboard/Scancode/F1");
}

void ApplicationImpl::executeKeyboardShortcuts() {
  for (auto &&canvasPtr : canvases) {
    if (canvasPtr.expired()) {
      continue;
    }

    auto canvas = canvasPtr.lock();
    auto &queue = canvas->getButtonEvents();

    // note: since we are not using input buffer, the time period (0.1f) does
    // not matter
    mHotkeysManager.update(&queue, 0.1f);

    if (mHotkeysManager.isButtonPressed("Toggle Debugger")) {
      if (canvas->getImmediateGui()) {
        canvas->attachImmediateGui(nullptr);
      } else {
        Application::getInstance()->openDebugger(canvas);
      }
    }
  }
}

void ApplicationImpl::mainLoop() {
  logInfo("entering main loop");

  while (hasCanvas() && mRaisedErrorMessage.isEmpty()) {
#ifdef __SWITCH__
    if (!appletMainLoop()) {
      break;
    }
#endif

    mDebugMetrics->frameTicker.tick();
#ifdef LUNA_USE_SDL
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        logInfo("sdl quit event received");
        return;
      }

      for (auto &&canvas : canvases) {
        if (canvas.expired()) {
          continue;
        }

        if (canvas.lock()->getImpl()->processSdlEvent(&event)) {
          break;
        }
      }
    }
#endif
#ifdef LUNA_USE_GLFW
    glfwPollEvents();

    for (auto &&canvas : canvases) {
      if (canvas.expired() || !canvas.lock()->mImpl->glfw.window) {
        continue;
      }

      if (glfwWindowShouldClose(canvas.lock()->mImpl->glfw.window)) {
        canvas.lock()->close();
      }
    }
#endif
    executeKeyboardShortcuts();
    mIntervalManager.executePendingIntervals();

    mAudioManager.update();

    mDebugMetrics->frameTicker.measure();

    mDebugMetrics->renderTicker.tick();
    // logDebug("rendering canvases");
    for (auto &&canvas : canvases) {
      if (canvas.expired()) {
        continue;
      }

      canvas.lock()->render();
    }
    mDebugMetrics->renderTicker.measure();

    // logDebug("syncing canvases");
    for (auto &&canvas : canvases) {
      if (canvas.expired()) {
        continue;
      }

      canvas.lock()->sync();
    }

    ++mDebugMetrics->framesElapsed;
  }

  for (auto &&canvas : canvases) {
    if (canvas.expired()) {
      continue;
    }

    canvas.lock()->close();
  }

  if (!mRaisedErrorMessage.isEmpty()) {
#ifdef __SWITCH__
    Terminal::init();
    logError("{}", mRaisedErrorMessage);
    while (appletMainLoop()) {
      Terminal::update();
    }
#endif
  }

  logInfo("existing main loop");
}

void ApplicationImpl::shutDown() {
  for (auto &&canvas : canvases) {
    if (canvas.expired()) {
      continue;
    }

    canvas.lock()->close();
  }

#ifdef LUNA_USE_SDL
  logInfo("shutting down SDL");
  SDL_Quit();
#endif

#ifdef LUNA_USE_GLFW
  logInfo("shutting down GLFW");
  glfwTerminate();
#endif
}

bool ApplicationImpl::hasCanvas() {
  for (auto &&canvas : canvases) {
    if (!canvas.expired()) {
#ifdef LUNA_USE_GLFW
      if (canvas.lock()->mImpl->glfw.window != nullptr) {
        return true;
      }
#else
      return true;
#endif
    }
  }

  return false;
}

#ifdef LUNA_USE_SDL
std::shared_ptr<Canvas> ApplicationImpl::getCanvasBySdlWindowId(Uint32 windowId
) {
  for (auto &&canvas : canvases) {
    if (canvas.expired()) {
      continue;
    }

    auto canvasPtr = canvas.lock();

    if (canvasPtr->getImpl()->sdl.window == nullptr) {
      continue;
    }

    auto canvasWindowId = SDL_GetWindowID(canvasPtr->getImpl()->sdl.window);

    if (canvasWindowId == windowId) {
      return canvasPtr;
    }
  }

  return nullptr;
}

void ApplicationImpl::pushSdlEvent(SDL_Event *event) { SDL_PushEvent(event); }
#endif

Application::Application(int argc, char **argv)
    : mImpl{std::make_unique<ApplicationImpl>(this)} {
  mImpl->mArgs.reserve(static_cast<std::size_t>(argc));

  for (int i = 0; i < argc; ++i) {
    mImpl->mArgs.emplace_back(argv[i]);
  }

  mImpl->mName = APP_PRODUCT_NAME;
}

Application::~Application() = default;

Application *Application::getInstance() { return gSingletonApp; }

int Application::run() {
  String assetsPath = Application::getInstance()->getOptionValue("assets");

  if (!assetsPath.isEmpty()) {
    setAssetsPath(assetsPath);
  }

  initSystem();

  printCompiler();
  printDefines();
  printArguments(mImpl->mArgs);

  mImpl->mDebugMetrics = std::make_shared<Internal::DebugMetrics>();

  if (mImpl->mReadyCallback) {
    logInfo("calling ready callback");
    mImpl->mReadyCallback();
  }

  if (mImpl->hasCanvas()) {
    mImpl->mAudioManager.init();
    mImpl->mainLoop();
    mImpl->mAudioManager.free();
  }

  mImpl->shutDown();

  return 0;
}

void Application::whenReady(std::function<void()> callback) {
  mImpl->mReadyCallback = callback;
}

void Application::addInterval(
    int ratePerSecond, std::function<void(float)> callback
) {
  mImpl->mIntervalManager.addInterval(ratePerSecond, callback);
}

void Application::addVsync(std::function<void(float)> callback) {
  mImpl->mIntervalManager.addAlways(callback);
}

int Application::getOptionIndex(const String &name) const {
  bool isLong = name.getLength() > 1;

  for (int index = 1; index < static_cast<int>(mImpl->mArgs.size()); ++index) {
    auto &arg = mImpl->mArgs[index];

    if (arg[0] != '-') {
      // not an option
      continue;
    }

    if (isLong) {
      if (arg[1] != '-') {
        continue;
      }

      if (name == arg.subString(2)) {
        return index;
      }
    } else {
      if (arg[1] == '-') {
        continue;
      }

      if (arg.subString(1).indexOf(name).has_value()) {
        return index;
      }
    }
  }

  return -1;
}

bool Application::hasOption(const String &name) const {
  return this->getOptionIndex(name) != -1;
}

String Application::getOptionValue(const String &name) const {
  int optIndex = this->getOptionIndex(name);

  if (optIndex == -1 || optIndex + 1 >= static_cast<int>(mImpl->mArgs.size())) {
    return String();
  }

  return mImpl->mArgs.at(optIndex + 1);
}

Filesystem::Path Application::getAssetsPath() const {
  return mImpl->mPathManager.getAssetsPath();
}

void Application::setAssetsPath(Filesystem::Path assetsPath) {
  mImpl->mPathManager.setAssetsPath(assetsPath);
}

unsigned int Application::getProcessorCount() {
#ifdef __MINGW32__
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#else
  return std::thread::hardware_concurrency();
#endif
}

unsigned long Application::getPhysicalMemoryUsage() {
#ifdef __linux__
  FILE *fp = fopen("/proc/self/status", "rt");
  unsigned long result = 0;
  char line[128];

  while (fgets(line, 128, fp) != nullptr) {
    if (strncmp(line, "VmRSS:", 6) == 0) {
      auto i = strlen(line);
      const char *p = line;
      while (*p < '0' || *p > '9') {
        p++;
      }
      line[i - 3] = '\0'; // remove " kB"
      result = static_cast<decltype(result)>(atol(p) * 1024);
      break;
    }
  }

  fclose(fp);
  return result;
#elif defined(_WIN32)
  PROCESS_MEMORY_COUNTERS_EX pmc;
  GetProcessMemoryInfo(
      GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc)
  );
  return static_cast<unsigned long>(pmc.WorkingSetSize);
#else
  return 0;
#endif
}

unsigned long Application::getVirtualMemoryUsage() {
#ifdef __linux__
  FILE *fp = fopen("/proc/self/status", "rt");
  unsigned long result = 0;
  char line[128];

  while (fgets(line, 128, fp) != nullptr) {
    if (strncmp(line, "VmSize:", 6) == 0) {
      auto i = strlen(line);
      const char *p = line;
      while (*p < '0' || *p > '9') {
        p++;
      }
      line[i - 3] = '\0'; // remove " kB"
      result = static_cast<decltype(result)>(atol(p) * 1024);
      break;
    }
  }

  fclose(fp);
  return result;
#elif defined(_WIN32)
  PROCESS_MEMORY_COUNTERS_EX pmc;
  GetProcessMemoryInfo(
      GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc)
  );
  return static_cast<unsigned long>(pmc.PrivateUsage);
#else
  return 0;
#endif
}

std::shared_ptr<Canvas> Application::makeCanvas(const Vector2i &size) {
  logInfo("creating canvas {}x{}", size.x(), size.y());

  auto canvas = std::make_shared<Canvas>(size);

  mImpl->canvases.emplace_back(canvas);

  return canvas;
}

std::list<std::shared_ptr<Canvas>> Application::getOpenCanvases() {
  std::list<std::shared_ptr<Canvas>> canvases;

  for (auto &&canvas : mImpl->canvases) {
    canvases.emplace_back(canvas);
  }

  return canvases;
}

void Application::raiseCriticalError(const String &message) {
  if (mImpl->mRaisedErrorMessage.isEmpty()) {
    mImpl->mRaisedErrorMessage = message;
  }
}

String Application::getDefaultVideoDriver() const {
  String driverName = Application::getInstance()->getOptionValue("video");

  if (driverName.isEmpty()) {
#ifdef N64
    return "n64";
#endif
#ifdef LUNA_USE_OPENGL
    return "opengl";
#else
    return "sdl";
#endif
  }

  return driverName;
}

void Application::setName(const String &name) { mImpl->mName = name; }

const String &Application::getName() const { return mImpl->mName; }

std::shared_ptr<AudioNode> Application::getAudioDestinationNode() const {
  return mImpl->mAudioManager.getDestinationNode();
}

void Application::openDebugger([[maybe_unused]] std::shared_ptr<Canvas> canvas
) {
#ifdef LUNA_USE_IMGUI
  if (!canvas->getImmediateGui()) {
    canvas->attachImmediateGui(
        std::make_unique<Internal::DebugGui>(mImpl->mDebugMetrics)
    );
  }
#endif
}

ApplicationImpl *Application::getImpl() const { return mImpl.get(); }
