#include <libluna/config.h>

#include <libluna/Application.hpp>

#ifdef __SWITCH__
#include <switch.h>
#endif

#ifdef NDS
#include <nds.h>
#endif

#ifdef N64
#include <libdragon.h>
#endif

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

#ifdef LUNA_WINDOW_GLFW
#include <GLFW/glfw3.h>
#endif

#ifdef LUNA_IMGUI
#include <imgui/imgui.h>
#include <libluna/Internal/DebugGui.hpp>
#endif

#include <libluna/Audio/AudioManager.hpp>
#include <libluna/Clock.hpp>
#include <libluna/Console.hpp>
#include <libluna/ImmediateGui.hpp>
#include <libluna/InputManager.hpp>
#include <libluna/Internal/DebugMetrics.hpp>
#include <libluna/IntervalManager.hpp>
#include <libluna/Logger.hpp>
#include <libluna/PathManager.hpp>
#include <libluna/Performance/Ticker.hpp>
#include <libluna/Platform.hpp>

using namespace Luna;
using namespace Luna::Audio;

static Application *gSingletonApp;

static void initSystem() {
  Console::init();

#ifdef __SWITCH__
  logInfo("initializing romfs");

  Result rc = romfsInit();

  if (R_FAILED(rc)) {
    logWarn("romfsInit failed: {}", rc);
  }
#endif
#ifdef N64
  dfs_init(DFS_DEFAULT_LOCATION);
  joypad_init();
#endif

#ifdef LUNA_WINDOW_SDL2
  logInfo("initializing SDL");

  auto result = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_AUDIO);

  if (result != 0) {
    logError(SDL_GetError());
  }
#endif

#ifdef LUNA_WINDOW_GLFW
  logInfo("initializing GLFW");
  if (!glfwInit()) {
    logError("unable to initialize GLFW");
  }
#endif
}

static void printCompiler() {
  for (auto &&info : Platform::getCompilerInfo()) {
    Console::writeLine(info);
  }
}

static void printDefines() {
  if (Platform::isDebug()) {
    Console::writeLine("debug build (NDBEUG undefined)");
  } else {
    Console::writeLine("release build (NDEBUG defined)");
  }

  Console::writeLine("System: {}", Platform::getSystemName());
}

static void printArguments(const std::vector<String> &args) {
  for (std::size_t i = 0; i < args.size(); ++i) {
    Console::writeLine("Argument {}: {}", i, args[i].c_str());
  }
}

void Application::executeKeyboardShortcuts() {
  for (auto &&canvas : mCanvases) {
    auto &queue = canvas.getButtonEvents();

    // note: since we are not using input buffer, the time period (0.1f) does
    // not matter
    mHotkeysManager.update(&queue, 0.1f);

    if (mHotkeysManager.isButtonPressed("Toggle Debugger")) {
      if (canvas.getImmediateGui()) {
        canvas.attachImmediateGui(nullptr);
      } else {
        Application::getInstance()->openDebugger(&canvas);
      }
    }
  }
}

void Application::mainLoop() {
  logInfo("entering main loop");

  mIntervalManager.addAlways([this](float deltaTime) {
    this->update(deltaTime);
  });

  while (hasCanvas() && mRaisedErrorMessage.isEmpty()) {
#ifdef __SWITCH__
    if (!appletMainLoop()) {
      break;
    }
#endif
#ifdef NDS
    swiWaitForVBlank();
#endif

    mDebugMetrics->frameTicker.tick();
    processEvents();

    if (!hasCanvas()) {
      // quit application
      break;
    }

    executeKeyboardShortcuts();
    mIntervalManager.executePendingIntervals();

    mAudioManager.update();

    mDebugMetrics->frameTicker.measure();

    mDebugMetrics->renderTicker.tick();
    // logDebug("rendering canvases");
    for (auto &&canvas : mCanvases) {
      canvas.render();
    }
    mDebugMetrics->renderTicker.measure();

    // logDebug("syncing canvases");
    for (auto &&canvas : mCanvases) {
      canvas.sync();
    }

    ++mDebugMetrics->framesElapsed;
  }

  for (auto &&canvas : mCanvases) {
    canvas.close();
  }

  if (!mRaisedErrorMessage.isEmpty()) {
#ifdef __SWITCH__
    Console::init();
    logError("{}", mRaisedErrorMessage.c_str());
    while (appletMainLoop()) {
      Console::update();
    }
#endif
#ifdef NDS
    Console::init();
    logError("{}", mRaisedErrorMessage.c_str());
    while (true) {
      Console::update();
      swiWaitForVBlank();
    }
#endif
#ifdef N64
    Console::init();
    logError("{}", mRaisedErrorMessage.c_str());
    while (true) {
      Console::update();
    }
#endif
  }

  logInfo("exiting main loop");
}

void Application::processEvents() {
#ifdef LUNA_WINDOW_SDL2
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      logInfo("sdl quit event received");

      for (auto &&canvas : mCanvases) {
        canvas.close();
      }

      return;
    }

    for (auto &&canvas : mCanvases) {
      if (canvas.processSdlEvent(&event)) {
        break;
      }
    }
  }
#endif
#ifdef LUNA_WINDOW_GLFW
  glfwPollEvents();

  for (auto &&canvas : mCanvases) {
    if (!canvas.glfw.window) {
      continue;
    }

    if (glfwWindowShouldClose(canvas.glfw.window)) {
      canvas.close();
    }
  }
#endif
#ifdef N64
  for (auto &&canvas : mCanvases) {
    auto &events = canvas.getButtonEvents();

    joypad_poll();

    auto pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if (pressed.z) {
      events.push(ButtonEvent("Gamepad/N64/Z", true));
    }

    if (pressed.d_up) {
      events.push(ButtonEvent("Gamepad/N64/DPadUp", true));
    }

    if (pressed.d_down) {
      events.push(ButtonEvent("Gamepad/N64/DPadDown", true));
    }

    if (pressed.d_left) {
      events.push(ButtonEvent("Gamepad/N64/DPadLeft", true));
    }

    if (pressed.d_right) {
      events.push(ButtonEvent("Gamepad/N64/DPadRight", true));
    }

    if (pressed.c_up) {
      events.push(ButtonEvent("Gamepad/N64/CUp", true));
    }

    if (pressed.c_down) {
      events.push(ButtonEvent("Gamepad/N64/CDown", true));
    }

    if (pressed.c_left) {
      events.push(ButtonEvent("Gamepad/N64/CLeft", true));
    }

    if (pressed.c_right) {
      events.push(ButtonEvent("Gamepad/N64/CRight", true));
    }

    auto released = joypad_get_buttons_released(JOYPAD_PORT_1);

    if (released.z) {
      events.push(ButtonEvent("Gamepad/N64/Z", false));
    }

    if (released.d_up) {
      events.push(ButtonEvent("Gamepad/N64/DPadUp", false));
    }

    if (released.d_down) {
      events.push(ButtonEvent("Gamepad/N64/DPadDown", false));
    }

    if (released.d_left) {
      events.push(ButtonEvent("Gamepad/N64/DPadLeft", false));
    }

    if (released.d_right) {
      events.push(ButtonEvent("Gamepad/N64/DPadRight", false));
    }

    if (released.c_up) {
      events.push(ButtonEvent("Gamepad/N64/CUp", false));
    }

    if (released.c_down) {
      events.push(ButtonEvent("Gamepad/N64/CDown", false));
    }

    if (released.c_left) {
      events.push(ButtonEvent("Gamepad/N64/CLeft", false));
    }

    if (released.c_right) {
      events.push(ButtonEvent("Gamepad/N64/CRight", false));
    }

    return;
  }
#endif
}

void Application::shutDown() {
  for (auto &&canvas : mCanvases) {
    canvas.close();
  }

#ifdef LUNA_WINDOW_SDL2
  logInfo("shutting down SDL");
  SDL_Quit();
#endif

#ifdef LUNA_WINDOW_GLFW
  logInfo("shutting down GLFW");
  glfwTerminate();
#endif
}

bool Application::hasCanvas() {
  for (auto &&canvas : mCanvases) {
    if (!canvas.isClosed()) {
      return true;
    }
  }

  return false;
}

#ifdef LUNA_WINDOW_SDL2
Canvas *Application::getCanvasBySdlWindowId(Uint32 windowId) {
  for (auto &&canvas : mCanvases) {
    if (canvas.sdl.window == nullptr) {
      continue;
    }

    auto canvasWindowId = SDL_GetWindowID(canvas.sdl.window);

    if (canvasWindowId == windowId) {
      return &canvas;
    }
  }

  return nullptr;
}

void Application::pushSdlEvent(SDL_Event *event) { SDL_PushEvent(event); }
#endif

Application::Application(int argc, char **argv) {
  Clock::init();

  gSingletonApp = this;

  mHotkeysManager.setReturnUnused(true);
  mHotkeysManager.addButtonBinding("Toggle Debugger", "Keyboard/Scancode/F1");

  mArgs.reserve(static_cast<std::size_t>(argc));

  for (int i = 0; i < argc; ++i) {
    mArgs.emplace_back(argv[i]);
  }

  mName = APP_PRODUCT_NAME;
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
  printArguments(mArgs);

  mDebugMetrics = std::make_shared<Internal::DebugMetrics>();

  this->init();

  if (hasCanvas()) {
    mAudioManager.init();
    mainLoop();
    mAudioManager.free();
  }

  shutDown();

  return 0;
}

void Application::addInterval(
    int ratePerSecond, std::function<void(float)> callback
) {
  mIntervalManager.addInterval(ratePerSecond, callback);
}

void Application::addVsync(std::function<void(float)> callback) {
  mIntervalManager.addAlways(callback);
}

int Application::getOptionIndex(const String &name) const {
  bool isLong = name.getLength() > 1;

  for (int index = 1; index < static_cast<int>(mArgs.size()); ++index) {
    auto &arg = mArgs[index];

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

  if (optIndex == -1 || optIndex + 1 >= static_cast<int>(mArgs.size())) {
    return String();
  }

  return mArgs.at(optIndex + 1);
}

Filesystem::Path Application::getAssetsPath() const {
  return mPathManager.getAssetsPath();
}

void Application::setAssetsPath(Filesystem::Path assetsPath) {
  mPathManager.setAssetsPath(assetsPath);
}

Canvas *Application::makeCanvas(const Vector2i &size) {
  logInfo("creating canvas {}x{}", size.width, size.height);

  auto canvas = mCanvases.acquire(size);

  return canvas;
}

std::list<Canvas *> Application::getOpenCanvases() {
  std::list<Canvas *> canvases;

  for (auto &&canvas : canvases) {
    canvases.emplace_back(canvas);
  }

  return canvases;
}

void Application::raiseCriticalError(const String &message) {
  if (mRaisedErrorMessage.isEmpty()) {
    mRaisedErrorMessage = message;
  }
}

String Application::getDefaultVideoDriver() const {
  String driverName = Application::getInstance()->getOptionValue("video");

  if (driverName.isEmpty()) {
#ifdef N64
    return "n64";
#endif
#ifdef LUNA_RENDERER_OPENGL
    return "opengl";
#else
    return "sdl";
#endif
  }

  return driverName;
}

void Application::setName(const String &name) { mName = name; }

const String &Application::getName() const { return mName; }

AudioManager *Application::getAudioManager() { return &mAudioManager; }

AudioNodePtr Application::getAudioDestinationNode() const {
  return mAudioManager.getDestinationNode();
}

void Application::openDebugger([[maybe_unused]] Canvas *canvas
) {
#ifdef LUNA_IMGUI
  if (!canvas->getImmediateGui()) {
    canvas->attachImmediateGui(
        std::make_unique<Internal::DebugGui>(mDebugMetrics)
    );
  }
#endif
}
