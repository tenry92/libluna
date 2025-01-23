#include <cmath>
#include <cstring> // memcpy

#include <libluna/MemoryReader.hpp>

#include <libluna/Filesystem/FileReader.hpp>

using namespace Luna;

MemoryReader::MemoryReader(void* address, std::size_t size)
    : mAddress{address}, mSize{size} {}

MemoryReader::~MemoryReader() = default;

bool MemoryReader::isValid() const { return true; }

std::size_t MemoryReader::getSize() const { return mSize; }

bool MemoryReader::eof() const { return mPos >= mSize; }

std::size_t MemoryReader::tell() { return mPos; }

std::size_t MemoryReader::seek(std::size_t position) {
  mPos = std::min(mSize, position);

  return mPos;
}

std::size_t MemoryReader::seekRelative(int relativePosition) {
  return seek(mPos + relativePosition);
}

std::size_t MemoryReader::read(
  uint8_t* buffer, std::size_t objectSize, std::size_t objectCount
) {
  auto endPos = mPos + objectSize * objectCount;

  if (endPos > mSize) {
    auto tooMuch = endPos - mSize;
    auto tooMuchObjects = (tooMuch + objectSize - 1) / objectSize;
    objectCount -= tooMuchObjects;
  }

  auto sourceAddress = reinterpret_cast<char*>(mAddress) + mPos;

  std::memcpy(buffer, sourceAddress, objectCount * objectSize);

  mPos += objectCount * objectSize;

  return objectCount;
}
