#include <cmath>
#include <cstring>

#include <libluna/MemoryReader.hpp>

#include <libluna/Filesystem/FileReader.hpp>

using namespace Luna;

class MemoryReader::impl {
  public:
  void *mAddress;
  std::size_t mSize;
  std::size_t mPos{0};
};

MemoryReader::MemoryReader(void *address, std::size_t size)
    : mImpl{std::make_unique<impl>()} {
  mImpl->mAddress = address;
  mImpl->mSize = size;
}

MemoryReader::~MemoryReader() = default;

bool MemoryReader::isValid() const { return true; }

std::size_t MemoryReader::getSize() const { return mImpl->mSize; }

bool MemoryReader::eof() const { return mImpl->mPos >= mImpl->mSize; }

std::size_t MemoryReader::tell() { return mImpl->mPos; }

std::size_t MemoryReader::seek(std::size_t position) {
  mImpl->mPos = std::min(mImpl->mSize, position);

  return mImpl->mPos;
}

std::size_t MemoryReader::seekRelative(int relativePosition) {
  return seek(mImpl->mPos + relativePosition);
}

std::size_t MemoryReader::read(
    uint8_t *buffer, std::size_t objectSize, std::size_t objectCount
) {
  auto endPos = mImpl->mPos + objectSize * objectCount;

  if (endPos > mImpl->mSize) {
    auto tooMuch = endPos - mImpl->mSize;
    auto tooMuchObjects = (tooMuch + objectSize - 1) / objectSize;
    objectCount -= tooMuchObjects;
  }

  auto sourceAddress = reinterpret_cast<char *>(mImpl->mAddress) + mImpl->mPos;

  std::memcpy(buffer, sourceAddress, objectCount * objectSize);

  mImpl->mPos += objectCount * objectSize;

  return objectCount;
}
