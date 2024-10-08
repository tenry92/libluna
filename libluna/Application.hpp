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
#include <libluna/Resource.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/String.hpp>

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
  class Application final {
    public:
    /**
     * @brief Create application.
     *
     * This is not doing any setup until `run()` is called.
     */
    Application(int argc, char **argv);

    ~Application();

    static Application *getInstance();

    int run();
    void whenReady(std::function<void()> callback);
    void addInterval(int ratePerSecond, std::function<void(float)> callback);
    void addVsync(std::function<void(float)> callback);
    int getOptionIndex(const String &name) const;
    bool hasOption(const String &name) const;

    /**
     * @brief Get command-line option value.
     *
     * If you want to get the value of `--assets`, you check for `name`
     *
     * @param name Parameter name, excluding preceding dashes.
     */
    String getOptionValue(const String &name) const;

    Filesystem::Path getAssetsPath() const;

    void setAssetsPath(Filesystem::Path assetsPath);

    std::shared_ptr<Canvas> makeCanvas(const Vector2i &size);
    std::list<std::shared_ptr<Canvas>> getOpenCanvases();

    void raiseCriticalError(const String &message);

    String getDefaultVideoDriver() const;

    /**
     * @brief Set application name.
     */
    void setName(const String &name);

    /**
     * @brief Get application name.
     */
    const String &getName() const;

    Audio::AudioManager *getAudioManager();

    Audio::AudioNodePtr getAudioDestinationNode() const;

    void openDebugger(std::shared_ptr<Canvas> canvas);

#ifdef LUNA_WINDOW_SDL2
    std::shared_ptr<Canvas> getCanvasBySdlWindowId(Uint32 windowId);
    void pushSdlEvent(SDL_Event *event);
#endif

    private:
    void executeKeyboardShortcuts();

    void mainLoop();
    void processEvents();
    void shutDown();
    bool hasCanvas();

    std::vector<String> mArgs;
    std::function<void()> mReadyCallback;
    std::list<std::weak_ptr<Canvas>> mCanvases;
    String mRaisedErrorMessage;
    IntervalManager mIntervalManager;
    Audio::AudioManager mAudioManager;
    PathManager mPathManager;
    String mName;
    std::shared_ptr<Internal::DebugMetrics> mDebugMetrics;
    InputManager mHotkeysManager;
  };
} // namespace Luna
