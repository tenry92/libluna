#pragma once

#include <memory>

#include <libluna/InputStream.hpp>

namespace Luna {
  class ResourceReader final : public InputStream {
    public:
    ResourceReader(const char *name);
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
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
