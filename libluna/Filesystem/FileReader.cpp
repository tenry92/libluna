#include <libluna/Filesystem/FileReader.hpp>

#include <fstream>
#include <stdexcept>

#include <fmt/format.h>

using namespace Luna::Filesystem;

FileReaderPtr FileReader::make(const Path& filename, Endian::Endian endian) {
  return FileReaderPtr(new FileReader(filename, endian));
}

FileReader::FileReader(const Path& filename, Endian::Endian endian) : mEndian(endian) {
  auto rawPath = filename.getRawPath();
  mStream = std::ifstream(rawPath.c_str(), std::ios::binary);

  if (!mStream.good()) {
    throw std::runtime_error(
      fmt::format("unable to open file \"{}\" for reading", rawPath.c_str())
    );
  }

  mStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  mStream.seekg(0, std::ios_base::end);
  mSize = static_cast<std::size_t>(mStream.tellg());
  mStream.seekg(0, std::ios_base::beg);
}

FileReader::~FileReader() = default;

bool FileReader::isValid() const { return !mPath.isEmpty(); }

std::size_t FileReader::getSize() const { return mSize; }

bool FileReader::eof() const { return mStream.eof(); }

std::size_t FileReader::tell() {
  return static_cast<std::size_t>(mStream.tellg());
}

std::size_t FileReader::seek(std::size_t position) {
  mStream.seekg(position);

  return tell();
}

std::size_t FileReader::seekRelative(int position) {
  if (position > static_cast<int>(getSize())) {
    position = static_cast<int>(tell());
  } else if (position < 0) {
    position = 0;
  }

  mStream.seekg(position, std::ios_base::cur);

  return tell();
}

std::size_t FileReader::read(
  std::uint8_t* buffer, std::size_t objectSize, std::size_t objectCount
) {
  int startPos = static_cast<int>(mStream.tellg());

  if (startPos + objectSize * objectCount > mSize) {
    objectCount = (mSize - startPos) / objectSize;
  }

  mStream.read(reinterpret_cast<char*>(buffer), objectSize * objectCount);
  int endPos = static_cast<int>(mStream.tellg());

  if (endPos == -1) {
    // error
    return 0;
  }

  if (mEndian != Endian::getEndian()) {
    for (std::size_t i = 0; i < objectCount; i++) {
      switch (objectSize) {
      case 2: {
        auto* val = reinterpret_cast<uint16_t*>(buffer + i * objectSize);
        *val = Endian::swapEndian(*val);
        break;
      }
      case 4: {
        auto* val = reinterpret_cast<uint32_t*>(buffer + i * objectSize);
        *val = Endian::swapEndian(*val);
        break;
      }
      case 8: {
        auto* val = reinterpret_cast<uint64_t*>(buffer + i * objectSize);
        *val = Endian::swapEndian(*val);
        break;
      }
      default:
        // do nothing for unsupported sizes
        break;
      }
    }
  }

  return (endPos - startPos) / objectSize;
}
