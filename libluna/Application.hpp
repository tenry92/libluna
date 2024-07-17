#pragma once

#include <functional>
#include <memory>

#include <libluna/Audio/AudioManager.hpp>
#include <libluna/Audio/AudioNode.hpp>
#include <libluna/Canvas.hpp>
#include <libluna/Filesystem/Path.hpp>
#include <libluna/Resource.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/String.hpp>

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

    Audio::AudioManager *getAudioManager() const;

    Audio::AudioNodePtr getAudioDestinationNode() const;

    void openDebugger(std::shared_ptr<Canvas> canvas);

    ApplicationImpl *getImpl() const;

    private:
    std::unique_ptr<ApplicationImpl> mImpl;
  };
} // namespace Luna
