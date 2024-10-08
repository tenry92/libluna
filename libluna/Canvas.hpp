#pragma once

#include <libluna/config.h>

#include <map>
#include <queue>
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

#include <libluna/ButtonEvent.hpp>
#include <libluna/Camera2d.hpp>
#include <libluna/Camera3d.hpp>
#include <libluna/Canvas.hpp>
#include <libluna/Color.hpp>
#include <libluna/Command.hpp>
#include <libluna/Image.hpp>
#include <libluna/ImmediateGui.hpp>
#include <libluna/Internal/GraphicsMetrics.hpp>
#include <libluna/Stage.hpp>
#include <libluna/String.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class AbstractRenderer;

  /**
   * @brief Abstraction for windows (computer) and fullscreen displays.
   */
  class Canvas {
    public:
    Canvas(const Vector2i &size);
    ~Canvas();

    void close();

    void setVideoDriver(const String &name);

    void attachImmediateGui(std::unique_ptr<ImmediateGui> gui);
    ImmediateGui *getImmediateGui() const;

    void setStage(std::shared_ptr<Stage> stage);
    std::shared_ptr<Stage> getStage() const;

    void setCamera2d(const Camera2d &camera);
    Camera2d getCamera2d() const;

    void setCamera3d(const Camera3d &camera);
    Camera3d getCamera3d() const;

    void setBackgroundColor(ColorRgb color);
    ColorRgb getBackgroundColor() const;

    ImagePtr captureScreenshot();

    void render();
    void sync();

    std::queue<ButtonEvent> &getButtonEvents();

    /**
     * @brief Get the current value of an axis input.
     *
     * The axis may be the horizontal or vertical position of a joystick or the
     * value of a trigger button.
     */
    float getInputAxis(const String &name);

    bool isClosed() const;

    Vector2i getOriginalSize() const;

    Internal::GraphicsMetrics getMetrics();

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

    private:
    void createWindow(bool opengl);
    void processCommandQueue();

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
} // namespace Luna
