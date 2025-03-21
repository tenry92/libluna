#pragma once

#include <cstdint>
#include <fstream>
#include <memory>

#include <libluna/Filesystem/Path.hpp>
#include <libluna/InputStream.hpp>

namespace Luna::Filesystem {
  class FileReader;
  using FileReaderPtr = std::unique_ptr<FileReader>;

  /**
   * @brief Reading files from the filesystem.
   *
   * @ingroup streams
   */
  class FileReader final : public InputStream {
    public:
    /**
     * @brief Create a new binary file reader for the file at the given path.
     */
    static FileReaderPtr make(const Path& filename);
    ~FileReader();

    bool isValid() const final override;
    std::size_t getSize() const final override;
    bool eof() const final override;
    std::size_t tell() final override;
    std::size_t seek(std::size_t position) final override;
    std::size_t seekRelative(int relativePosition) final override;
    std::size_t read(
      std::uint8_t* buffer, std::size_t objectSize, std::size_t objectCount
    ) final override;
    using InputStream::read;

    private:
    explicit FileReader(const Path& filename);
    Path mPath;
    std::ifstream mStream;
    std::size_t mSize;
  };
} // namespace Luna::Filesystem
