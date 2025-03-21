#include <libluna/config.h>

#include <libluna/Application.hpp>
#include <libluna/Internal/Keyboard.hpp>

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
using namespace Luna::Internal;

static Application* gSingletonApp;

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
  for (auto&& info : Platform::getCompilerInfo()) {
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

static void printArguments(const std::vector<String>& args) {
  for (std::size_t i = 0; i < args.size(); ++i) {
    Console::writeLine("Argument {}: {}", i, args[i].c_str());
  }
}

void Application::handleButtonEvent(const ButtonEvent& event) {
  mHotkeysManager.handleButtonEvent(event);
}

void Application::executeKeyboardShortcuts() {
  for (auto&& canvas : mCanvases) {
    if (mHotkeysManager.isButtonPressed("Toggle Debugger")) {
      Application::getInstance()->toggleDebugger(&canvas);
    }
  }

  mHotkeysManager.update(0.1f);
}

void Application::mainLoop() {
  logInfo("entering main loop");

  mIntervalManager.addAlways([this](float deltaTime) {
    if (mTimeScale > 0.f) {
      this->update(deltaTime * mTimeScale);
    } else if (mDoStep) {
      this->update(deltaTime);
      mDoStep = false;
    }
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
    for (auto&& canvas : mCanvases) {
      canvas.render();
    }
    mDebugMetrics->renderTicker.measure();

    for (auto&& canvas : mCanvases) {
      canvas.sync();
    }

    // logDebug("==============");
    // logDebug("frame #{}", mDebugMetrics->framesElapsed);
    // logDebug("update time: {}ms; updates/second: {}", (mDebugMetrics->frameTicker.getTickDuration() * 1000), mDebugMetrics->frameTicker.getTicksPerSecond());
    // logDebug("render time: {}ms; renders/second: {}", (mDebugMetrics->renderTicker.getTickDuration() * 1000), mDebugMetrics->renderTicker.getTicksPerSecond());

    ++mDebugMetrics->framesElapsed;
  }

  for (auto&& canvas : mCanvases) {
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

      for (auto&& canvas : mCanvases) {
        canvas.close();
      }

      return;
    }

    bool eventHandledByImmediateGui = false;

    for (auto&& canvas : mCanvases) {
      if (canvas.sdlEventTargetsThis(&event)) {
        eventHandledByImmediateGui = canvas.sendSdlEventToImmediateGui(&event);

        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
          canvas.close();
        }

        break;
      }
    }

    if (eventHandledByImmediateGui) {
      continue;
    }

    switch (event.type) {
      case SDL_KEYDOWN: {
        auto keycodeName = Keyboard::sdlKeycodeToName(event.key.keysym.sym);

        if (!keycodeName.isEmpty()) {
          handleButtonEvent(ButtonEvent(keycodeName, true));
        }

        auto scancodeName = Keyboard::sdlScancodeToName(event.key.keysym.scancode);

        if (!scancodeName.isEmpty()) {
          handleButtonEvent(ButtonEvent(scancodeName, true));
        }

        break;
      }
      case SDL_KEYUP: {
        auto keycodeName = Keyboard::sdlKeycodeToName(event.key.keysym.sym);

        if (!keycodeName.isEmpty()) {
          handleButtonEvent(ButtonEvent(keycodeName, false));
        }

        auto scancodeName = Keyboard::sdlScancodeToName(event.key.keysym.scancode);

        if (!scancodeName.isEmpty()) {
          handleButtonEvent(ButtonEvent(scancodeName, false));
        }

        break;
      }
    }
  }
#endif
#ifdef LUNA_WINDOW_GLFW
  glfwPollEvents();

  for (auto&& canvas : mCanvases) {
    if (!canvas.glfw.window) {
      continue;
    }

    if (glfwWindowShouldClose(canvas.glfw.window)) {
      canvas.close();
    }
  }
#endif
#ifdef N64
  for (auto&& canvas : mCanvases) {
    auto& events = canvas.getButtonEvents();

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
  for (auto&& canvas : mCanvases) {
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
  for (auto&& canvas : mCanvases) {
    if (!canvas.isClosed()) {
      return true;
    }
  }

  return false;
}

#ifdef LUNA_WINDOW_SDL2
Canvas* Application::getCanvasBySdlWindowId(Uint32 windowId) {
  for (auto&& canvas : mCanvases) {
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

void Application::pushSdlEvent(SDL_Event* event) { SDL_PushEvent(event); }
#endif

Application::Application(int argc, char** argv) {
#ifdef N64
#ifndef NDEBUG
  debug_init_usblog();
  debug_init_isviewer();
#endif
#endif

  Clock::init();

  gSingletonApp = this;

  mHotkeysManager.addButtonBinding("Toggle Debugger", "Keyboard/Scancode/F1");

  mArgs.reserve(static_cast<std::size_t>(argc));

  for (int i = 0; i < argc; ++i) {
    mArgs.emplace_back(argv[i]);
  }

  mName = APP_PRODUCT_NAME;
}

Application::Application(const String& name, int argc, char** argv) : Application(argc, argv) {
  mName = name;
}

Application::~Application() = default;

Application* Application::getInstance() { return gSingletonApp; }

int Application::run() {
  SDL_SetHint(SDL_HINT_APP_NAME, mName.c_str());

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

int Application::getOptionIndex(const String& name) const {
  bool isLong = name.getLength() > 1;

  for (int index = 1; index < static_cast<int>(mArgs.size()); ++index) {
    auto& arg = mArgs[index];

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

bool Application::hasOption(const String& name) const {
  return this->getOptionIndex(name) != -1;
}

String Application::getOptionValue(const String& name) const {
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

Canvas* Application::createCanvas(const Vector2i& size) {
  logInfo("creating canvas {}x{}", size.width, size.height);

  auto canvas = mCanvases.acquire(size);

  return canvas;
}

std::list<Canvas*> Application::getOpenCanvases() {
  std::list<Canvas*> canvases;

  for (auto&& canvas : mCanvases) {
    canvases.emplace_back(&canvas);
  }

  return canvases;
}

void Application::raiseCriticalError(const String& message) {
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

void Application::setName(const String& name) { mName = name; }

const String& Application::getName() const { return mName; }

AudioManager* Application::getAudioManager() { return &mAudioManager; }

AudioNodePtr Application::getAudioDestinationNode() const {
  return mAudioManager.getDestinationNode();
}

void Application::openDebugger([[maybe_unused]] Canvas* canvas) {
#ifdef LUNA_IMGUI
  if (!isDebuggerOpen(canvas)) {
    canvas->attachImmediateGui(
      std::make_unique<Internal::DebugGui>(mDebugMetrics)
    );
  }
#endif
}

void Application::closeDebugger(Canvas* canvas) {
#ifdef LUNA_IMGUI
  for (auto&& gui : canvas->getImmediateGuis()) {
    if (dynamic_cast<Internal::DebugGui*>(gui)) {
      canvas->detachImmediateGui(gui);
      break;
    }
  }
#endif
}

void Application::toggleDebugger(Canvas* canvas) {
  if (isDebuggerOpen(canvas)) {
    closeDebugger(canvas);
  } else {
    openDebugger(canvas);
  }
}

bool Application::isDebuggerOpen([[maybe_unused]] Canvas* canvas) {
#ifdef LUNA_IMGUI
  for (auto&& gui : canvas->getImmediateGuis()) {
    if (dynamic_cast<Internal::DebugGui*>(gui)) {
      return true;
    }
  }
#endif

  return false;
}

float Application::getTimeScale() const { return mTimeScale; }

void Application::setTimeScale(float timeScale) {
  mTimeScale = timeScale;
}

void Application::step() { mDoStep = true; }
