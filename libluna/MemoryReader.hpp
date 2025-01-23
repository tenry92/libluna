#pragma once

#include <libluna/InputStream.hpp>

namespace Luna {
  class MemoryReader final : public InputStream {
    public:
    MemoryReader(void *address, std::size_t size);
    ~MemoryReader();

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
    void *mAddress;
    std::size_t mSize;
    std::size_t mPos{0};
  };
} // namespace Luna
