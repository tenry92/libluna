#pragma once

#include <libluna/config.h>

#include <functional>
#include <list>
#include <memory>
#include <queue>
#include <vector>

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

#include <libluna/Application.hpp>
#include <libluna/Canvas.hpp>
#include <libluna/InputManager.hpp>
#include <libluna/Internal/DebugMetrics.hpp>
#include <libluna/IntervalManager.hpp>
#include <libluna/PathManager.hpp>

class Luna::ApplicationImpl {
  public:
  ApplicationImpl(Application *app);

  void executeKeyboardShortcuts();

  void mainLoop();
  void shutDown();
  bool hasCanvas();

#ifdef LUNA_WINDOW_SDL2
  std::shared_ptr<Canvas> getCanvasBySdlWindowId(Uint32 windowId);
  void pushSdlEvent(SDL_Event *event);
#endif

  std::vector<String> mArgs;
  std::function<void()> mReadyCallback;
  std::list<std::weak_ptr<Canvas>> canvases;
  String mRaisedErrorMessage;
  IntervalManager mIntervalManager;
  AudioManager mAudioManager;
  PathManager mPathManager;
  String mName;
  std::shared_ptr<Internal::DebugMetrics> mDebugMetrics;
  InputManager mHotkeysManager;
};
