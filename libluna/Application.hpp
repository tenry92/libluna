#pragma once

#include <functional>
#include <memory>

#include <libluna/AudioNode.hpp>
#include <libluna/Canvas.hpp>
#include <libluna/Filesystem/Path.hpp>
#include <libluna/Resource.hpp>
#include <libluna/Sprite.hpp>
#include <libluna/String.hpp>

namespace Luna {
  class ApplicationImpl;

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
    String getOptionValue(const String &name) const;

    Filesystem::Path getAssetsPath() const;

    void setAssetsPath(Filesystem::Path assetsPath);

    /**
     * \brief Get number of available processors.
     */
    unsigned int getProcessorCount();

    /**
     * \brief Get amount of used physical memory in bytes.
     */
    unsigned long getPhysicalMemoryUsage();

    /**
     * \brief Get amount of used virtual memory in bytes.
     */
    unsigned long getVirtualMemoryUsage();

    std::shared_ptr<Canvas> makeCanvas(const Vector2i &size);
    std::list<std::shared_ptr<Canvas>> getOpenCanvases();

    void raiseCriticalError(const String &message);

    String getDefaultVideoDriver() const;

    void setName(const String &name);
    const String &getName() const;

    std::shared_ptr<AudioNode> getAudioDestinationNode() const;

    void openDebugger(std::shared_ptr<Canvas> canvas);

    ApplicationImpl *getImpl() const;

    private:
    std::unique_ptr<ApplicationImpl> mImpl;
  };
} // namespace Luna
