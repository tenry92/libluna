#pragma once

#include <libluna/config.h>

#include <functional>
#include <list>
#include <queue>
#include <vector>

#ifdef LUNA_WINDOW_SDL2
#include <SDL2/SDL.h>
#endif

#include <libluna/Audio/AudioManager.hpp>
#include <libluna/Audio/AudioNode.hpp>
#include <libluna/Canvas.hpp>
#include <libluna/Filesystem/Path.hpp>
#include <libluna/InputManager.hpp>
#include <libluna/Internal/DebugMetrics.hpp>
#include <libluna/IntervalManager.hpp>
#include <libluna/PathManager.hpp>
#include <libluna/Pool.hpp>
#include <libluna/Resource.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/String.hpp>

/**
 * @mainpage Luna Game Engine
 *
 * ## Major classes
 *
 * These classes make the foundation of any Luna based application.
 * There is a single Application instance, usually one or two Canvas instances
 * and a Stage assigned to a Canvas.
 *
 * - @ref Luna::Application "Application" - The core class for the game engine.
 * - @ref Luna::Canvas "Canvas" - Abstraction for windows (computer) and fullscreen displays.
 * - @ref Luna::Stage "Stage" - Container for anything that can be drawn on a canvas
 *
 * ## Resources
 *
 * - @ref Luna::Resource "Resource" - Resource loading abstraction.
 * - @ref Luna::Image "Image" - Container for storing 2D image data.
 * - @ref Luna::Font "Font"
 * - @ref Luna::Mesh "Mesh" - 3D mesh containing vertices and faces and more.
 * - @ref Luna::Material "Material" - Material for 3D objects.
 *
 * ## On the stage
 *
 * - Drawables:
 *   - @ref Luna::Sprite "Sprite" - 2D object on the screen.
 *   - @ref Luna::Text "Text" - Text on the screen.
 *   - @ref Luna::Model "Model" - 3D model in the world.
 * - Manipulation:
 *   - @ref Luna::Camera2d "Camera2d" - Manipulating the 2D view.
 *   - @ref Luna::Camera3d "Camera3d" - Manipulating the 3D view.
 *
 * ## Images and colors
 *
 * - General color representation:
 *   - @ref Luna::ColorRgb "ColorRgb" - General RGBA color, using floats for each component.
 *   - @ref Luna::ColorHsv "ColorHsv" - General HSV color.
 *   - @ref Luna::ColorHsl "ColorHsl" - General HSL color.
 *   - @ref Luna::ColorYuv "ColorYuv" - General YUV color.
 * - Low-level color representation:
 *   - @ref Luna::ColorRgb16 "ColorRgb16" - 16-bit RGBA color.
 *   - @ref Luna::ColorRgb24 "ColorRgb24" - 24-bit RGB color.
 *   - @ref Luna::ColorRgb32 "ColorRgb32" - 32-bit RGBA color.
 *
 * ## Utilities
 *
 * - @ref Luna::Logger "Logger" - Logging utility.
 * - @ref Luna::Platform "Platform" - Platform detection.
 * - @ref Luna::String "String" - UTF-8 encoded string.
 * - @ref Luna::Vector2 "Vector2" - 2D vector utility.
 * - @ref Luna::Vector3 "Vector3" - 3D vector utility.
 * - @ref Luna::Matrix4 "Matrix4" - 4x4 matrix utility.
 * - @ref Luna::IdAllocator "IdAllocator" - ID allocator.
 *
 * ## Files
 *
 * - @ref Luna::Filesystem::Path "Path" - Path to a filesystem entry.
 * - @ref Luna::PathManager "PathManager" - Utility class for finding.
 * - @ref Luna::InputStream "InputStream" - Base class for binary input streams.
 *   - @ref Luna::Filesystem::FileReader "FileReader" - Read any file from the filesystem.
 *   - @ref Luna::MemoryReader "MemoryReader" - Read a section of memory like a file.
 *   - @ref Luna::ResourceReader "ResourceReader" - Read a resource.
 */

/**
 * @page app-structure Application structure
 *
 * The base structure for a Luna based application has the following structure:
 *
 * ```cpp
 * #include <libluna/Application.hpp>
 *
 * int main(int argc, char **argv) {
 *   Luna::Application app(argc, argv);
 *
 *   std::shared_ptr<Canvas> canvas;
 *
 *   app.whenReady([&]() {
 *     // create a canvas (window) for rendering
 *     canvas = app.makeCanvas(800, 600);
 *     canvas->setVideoDriver(app.getDefaultVideoDriver());
 *
 *     // define a sprite
 *     auto sprite = Luna::Sprite::make();
 *     sprite->setPosition({16, 32});
 *     sprite->setImage(...)
 *
 *     // create a stage, add the sprite to it and assign it to the canvas
 *     auto stage = std::make_shared<Luna::Stage>();
 *     stage->add(sprite);
 *     canvas->setStage(stage);
 *   });
 *
 *   app.addInterval(60, [&](float elapsedTime) {
 *     // update logic
 *   })
 *
 *   return app.run();
 * }
 * ```
 */

namespace Luna {
  class ApplicationImpl;

  /**
   * @brief This is the core class for the game engine.
   */
  class Application {
    public:
    /**
     * @brief Create application.
     *
     * This is not doing any setup until `run()` is called.
     */
    Application(int argc, char** argv);

    /**
     * @brief Create application with a name.
     *
     * This is not doing any setup until `run()` is called.
     *
     * @param name The user-friendly name of the application.
     */
    Application(const String& name, int argc, char** argv);

    virtual ~Application();

    static Application* getInstance();

    int run();
    void addInterval(int ratePerSecond, std::function<void(float)> callback);
    void addVsync(std::function<void(float)> callback);
    int getOptionIndex(const String& name) const;
    bool hasOption(const String& name) const;

    /**
     * @brief Get command-line option value.
     *
     * If you want to get the value of `--assets`, you check for `name`
     *
     * @param name Parameter name, excluding preceding dashes.
     */
    String getOptionValue(const String& name) const;

    Filesystem::Path getAssetsPath() const;

    void setAssetsPath(Filesystem::Path assetsPath);

    /**
     * @brief Create a canvas with the desired internal resolution.
     *
     * The internal framebuffer for 2D rendering does not change by default when
     * the window is resized.
     */
    Canvas* createCanvas(const Vector2i& size);

    std::list<Canvas*> getOpenCanvases();

    void raiseCriticalError(const String& message);

    String getDefaultVideoDriver() const;

    /**
     * @brief Set application name.
     */
    void setName(const String& name);

    /**
     * @brief Get application name.
     */
    const String& getName() const;

    Audio::AudioManager* getAudioManager();

    Audio::AudioNodePtr getAudioDestinationNode() const;

    void openDebugger(Canvas* canvas);

    void closeDebugger(Canvas* canvas);

    void toggleDebugger(Canvas* canvas);

    bool isDebuggerOpen(Canvas* canvas);

    float getTimeScale() const;

    /**
     * @brief Set the time scale.
     *
     * This affects the delta time passed to the update function.
     * A lower value will slow down the game.
     * A value of 0 will pause the game.
     */
    void setTimeScale(float timeScale);

    void step();

#ifdef LUNA_WINDOW_SDL2
    Canvas* getCanvasBySdlWindowId(Uint32 windowId);
    void pushSdlEvent(SDL_Event* event);
#endif

    protected:
    virtual void init() = 0;

    virtual void update(float deltaTime) = 0;

    virtual void handleButtonEvent(const ButtonEvent& event);

    private:
    void executeKeyboardShortcuts();

    void mainLoop();
    void processEvents();
    void shutDown();
    bool hasCanvas();

    std::vector<String> mArgs;
    Pool<Canvas, 4> mCanvases;
    String mRaisedErrorMessage;
    IntervalManager mIntervalManager;
    Audio::AudioManager mAudioManager;
    PathManager mPathManager;
    String mName;
    std::shared_ptr<Internal::DebugMetrics> mDebugMetrics;
    InputManager mHotkeysManager;
    float mTimeScale{1.0f};
    bool mDoStep{false}; ///< Step one frame if paused
  };
} // namespace Luna
