#pragma once

#include <libluna/Filesystem/FileReader.hpp>
#include <libluna/InputStream.hpp>

namespace Luna {
  class ResourceReader;
  using ResourceReaderPtr = std::unique_ptr<ResourceReader>;

  /**
   * @brief Read files from the assets.
   */
  class ResourceReader final : public InputStream {
    public:
    static ResourceReaderPtr make(const char *name);
    ~ResourceReader();

    bool isValid() const override;
    std::size_t getSize() const override;
    bool eof() const override;
    std::size_t tell() override;
    std::size_t seek(std::size_t position) override;
    std::size_t seekRelative(int relativePosition) override;
    std::size_t read(
        uint8_t *buffer, std::size_t objectSize, std::size_t objectCount
    ) override;

    private:
    ResourceReader(const char *name);
    std::unique_ptr<Filesystem::FileReader> fileReader;
  };
} // namespace Luna
