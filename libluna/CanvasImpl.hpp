#pragma once

#include <libluna/config.h>

#include <map>
#include <set>
#include <string>

#if defined(__linux__) || defined(_WIN32)
#define LUNA_THREADED_CANVAS
#endif

#ifdef LUNA_THREADED_CANVAS
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#endif

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

#ifdef LUNA_WINDOW_GLFW
#include <GLFW/glfw3.h>
#endif

#ifdef LUNA_WINDOW_EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#include <libluna/AbstractRenderer.hpp>
#include <libluna/Camera2d.hpp>
#include <libluna/Camera3d.hpp>
#include <libluna/Canvas.hpp>
#include <libluna/Command.hpp>
#include <libluna/Internal/GraphicsMetrics.hpp>

class Luna::CanvasImpl {
  public:
  inline CanvasImpl(const Vector2i &size) : mSize{size}, mOriginalSize{size} {}
  void setVideoDriver(const String &name);
  void createWindow(bool opengl);
  void processCommandQueue();
  Internal::GraphicsMetrics getMetrics();

  Canvas *mCanvas;
  Vector2i mSize;
  Vector2i mOriginalSize;
  std::shared_ptr<Stage> mStage;
  std::unique_ptr<AbstractRenderer> mRenderer;
  std::unique_ptr<ImmediateGui> mImmediateGui;
  Camera2d mCamera2d;
  Camera3d mCamera3d;
  ColorRgb mBackgroundColor;
  std::queue<ButtonEvent> mButtonEvents;
  std::map<std::string, float> mAxisValues;
  bool mClosed{false};

#ifdef LUNA_WINDOW_SDL2
  bool processSdlEvent(const SDL_Event *event);
  bool sdlEventTargetsThis(const SDL_Event *event);
  bool sendSdlEventToImmediateGui(const SDL_Event *event);
  struct {
    SDL_Window *window{nullptr};
#ifdef LUNA_RENDERER_OPENGL
    SDL_GLContext glContext{nullptr};
#endif
  } sdl;
#endif
#ifdef LUNA_WINDOW_GLFW
  struct {
    GLFWwindow *window{nullptr};
  } glfw;
#endif
#ifdef LUNA_WINDOW_EGL
  struct {
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
  } egl;
#endif

  std::queue<std::shared_ptr<Command>> mCommandQueue;

#ifdef LUNA_THREADED_CANVAS
  /**
   * @name Public Attributes (multi-threading)
   */
  ///@{
  void renderThread();

  /**
   * @brief The thread on which the graphics were initialized and a loop is
   * running.
   */
  std::thread mThread;

  /**
   * @brief The mutex for notifications via `mCv`.
   */
  std::mutex mMutex;

  /**
   * @brief Condition variable to either notify the thread to process the
   * command queue or the main thread if the command queue is empty.
   */
  std::condition_variable mCv;

  bool mExitRequested{false};
  ///@}
#endif
};
