#pragma once

#include <memory>

#include <libluna/Filesystem/Path.hpp>

namespace Luna {
  /**
   * @brief Utility class for finding paths to special directories.
   */
  class PathManager {
    public:
    PathManager();
    ~PathManager();

    /**
     * @brief Get path to the executable program binary.
     *
     * On Linux, this will be the ELF binary file.
     * On Windows, this will be the .exe file.
     * On other platforms, this is undefined.
     */
    Filesystem::Path getProgramPath() const;

    /**
     * The assets path is usually located at `share/assets` (UNIX style)
     * or just in `assets` next to the program.
     */
    Filesystem::Path getAssetsPath() const;

    /**
     * @brief Change the default assets path.
     *
     * If a relative path is provided, it's relative to the program path.
     */
    void setAssetsPath(Filesystem::Path assetsPath);

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
