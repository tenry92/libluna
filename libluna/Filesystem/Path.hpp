#pragma once

#include <stdint.h>

#include <memory>

#include <libluna/String.hpp>

namespace Luna::Filesystem {
  /**
   * \brief Path to a filesystem entry.
   * \ingroup Filesystem
   */
  class Path {
    public:
    Path();
    Path(const Path &other);
    Path(const String &path);
    ~Path();

    Path operator=(const Path &other);

    inline Path(const char *string) : Path(String(string)) {}

    String getRawPath() const;

    inline operator String() const { return getRawPath(); }

    /**
     * \brief Strip directory part of the path.
     *
     * For example, `path/to/file.txt` becomes `file.txt`.
     */
    String basename() const;

    /**
     * \brief Strip last component of the path.
     *
     * For example, `path/to/file.txt` becomes `path/to`.
     */
    inline String dirname() const { return up().getRawPath(); }

    /**
     * \brief Get filename extension (including period) or empty string.
     *
     * Returns `".html"` for `"file.html"` or empty string for `"file."` or
     * `"file"`.
     */
    String extname() const;

    /**
     * \brief Check if this path is absolute.
     *
     * An absolute path starts with a slash, on Windows it may also start with
     * a drive letter followed by a slash or backslach, e.g. \c "C:\\" .
     *
     * \see isRelative()
     */
    bool isAbsolute() const;

    /**
     * \brief Check if this path is relative.
     *
     * This returns the opposite of \ref isAbsolute(). Empty paths also count
     * as relative.
     *
     * \see isAbsolute()
     */
    inline bool isRelative() const { return !isAbsolute(); }

    bool isEmpty() const;

    /**
     * \brief Change directory.
     *
     * If \p other is absolute, the path is copied. Otherwise, it's applied
     * relatively to the current path.
     *
     * \par Example
     * \code{.cpp}
     * myPath.cd("sub/directory");
     * myPath.cd(".");             // changes nothing
     * myPath.cd("..");            // go up 1 folder
     * \endcode
     *
     * \see up()
     */
    Path cd(const Path &other) const;

    /**
     * \brief Change directory.
     *
     * This template allows for passing any number of arguments.
     *
     * \par Example
     * \code{.cpp}
     * myPath.cd("first", "second/third", "..");
     * \endcode
     */
    template <typename... ArgTypes>
    Path cd(const Path &first, ArgTypes... rest) const {
      return cd(first).cd(rest...);
    }

    /**
     * \brief Go up the specified number of levels.
     *
     * \param levels Number of levels to go up. A value of 1 equals \c cd("..")
     * .
     *
     * \see cd()
     */
    Path up(std::size_t levels = 1) const;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna::Filesystem
