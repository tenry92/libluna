#pragma once

#include <libluna/config.h>

#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>

#if defined(__linux__) || defined(_WIN32)
#define LUNA_THREADED_CANVAS
#endif

#ifdef N64
// Technically not defined, since N64 is not Linux or Windows,
// but code editor is not aware of this.
#undef LUNA_THREADED_CANVAS
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

  struct DisplayMode {
    Vector2i resolution{0, 0};
    bool fullscreen{false};
    String videoDriver;
  };

  /**
   * @brief Abstraction for windows (computer) and fullscreen displays.
   *
   * A canvas simply provides the interface to render graphics and receive input.
   * In order to see anything on the canvas, a stage must be assigned to it.
   * A stage contains all the objects that are drawn on the canvas, including
   * 2D sprites and 3D models.
   *
   * The same stage can be assigned to multiple canvases, for example to render
   * the same screen using different renderers or cameras.
   */
  class Canvas {
    public:
    /**
     * @brief Create a canvas with the desired internal resolution.
     *
     * The internal framebuffer for 2D rendering does not change by default when
     * the window is resized.
     */
    Canvas(const Vector2i& size);

    ~Canvas();

    void close();

    void setDisplayMode(DisplayMode mode);

    void attachImmediateGui(std::unique_ptr<ImmediateGui> gui);
    void detachImmediateGui(ImmediateGui* gui);
    std::list<ImmediateGui*> getImmediateGuis() const;

    void setStage(Stage* stage);
    Stage* getStage() const;

    void setCamera2d(const Camera2d& camera);
    Camera2d getCamera2d() const;

    void setCamera3d(const Camera3d& camera);
    Camera3d getCamera3d() const;

    void setBackgroundColor(ColorRgb color);
    ColorRgb getBackgroundColor() const;

    ImagePtr captureScreenshot();

    void render();
    void sync();

    /**
     * @brief Get the current value of an axis input.
     *
     * The axis may be the horizontal or vertical position of a joystick or the
     * value of a trigger button.
     */
    float getInputAxis(const String& name);

    bool isClosed() const;

    void setInternalResolution(Vector2i size);
    Vector2i getInternalResolution() const;

    static const std::list<Canvas*> getCanvasByStage(Stage* stage);

    Internal::GraphicsMetrics getMetrics();

#ifdef LUNA_WINDOW_SDL2
    bool sdlEventTargetsThis(const SDL_Event* event);
    bool sendSdlEventToImmediateGui(const SDL_Event* event);
    struct {
      SDL_Window* window{nullptr};
#ifdef LUNA_RENDERER_OPENGL
      SDL_GLContext glContext{nullptr};
#endif
    } sdl;
#endif
#ifdef LUNA_WINDOW_GLFW
    struct {
      GLFWwindow* window{nullptr};
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
    void createWindow(const DisplayMode& mode);
    void processCommandQueue();

    Vector2i mSize;
    Vector2i mInternalResolution;
    Stage* mStage;
    std::unique_ptr<AbstractRenderer> mRenderer;
    std::list<std::unique_ptr<ImmediateGui>> mImmediateGuis;
    Camera2d mCamera2d;
    Camera3d mCamera3d;
    ColorRgb mBackgroundColor;
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
