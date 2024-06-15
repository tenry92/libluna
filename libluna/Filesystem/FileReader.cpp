#include <libluna/Filesystem/FileReader.hpp>

#include <fstream>
#include <stdexcept>

using namespace Luna::Filesystem;

class FileReader::impl {
  public:
  Path path;
  std::ifstream stream;
  std::size_t size;
};

FileReaderPtr FileReader::make(const Path &filename) {
  return FileReaderPtr(new FileReader(filename));
}

FileReader::FileReader(const Path &filename) : mImpl{std::make_unique<impl>()} {
  mImpl->stream =
      std::ifstream(filename.getRawPath().c_str(), std::ios::binary);

  if (!mImpl->stream.good()) {
    throw std::runtime_error(String("unable to open file \"{}\" for reading")
                                 .format(filename.getRawPath())
                                 .s_str());
  }

  mImpl->stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  mImpl->stream.seekg(0, std::ios_base::end);
  mImpl->size = static_cast<std::size_t>(mImpl->stream.tellg());
  mImpl->stream.seekg(0, std::ios_base::beg);
}

FileReader::~FileReader() = default;

bool FileReader::isValid() const { return !mImpl->path.isEmpty(); }

std::size_t FileReader::getSize() const { return mImpl->size; }

bool FileReader::eof() const { return mImpl->stream.eof(); }

std::size_t FileReader::tell() {
  return static_cast<std::size_t>(mImpl->stream.tellg());
}

std::size_t FileReader::seek(std::size_t position) {
  mImpl->stream.seekg(position);

  return tell();
}

std::size_t FileReader::seekRelative(int position) {
  if (position > static_cast<int>(tell())) {
    position = static_cast<int>(tell());
  } else if (position < 0) {
    position = 0;
  }

  mImpl->stream.seekg(position, std::ios_base::cur);

  return tell();
}

std::size_t FileReader::read(
    std::uint8_t *buffer, std::size_t objectSize, std::size_t objectCount
) {
  int startPos = static_cast<int>(mImpl->stream.tellg());

  if (startPos + objectSize * objectCount > mImpl->size) {
    objectCount = (mImpl->size - startPos) / objectSize;
  }

  mImpl->stream.read(
      reinterpret_cast<char *>(buffer), objectSize * objectCount
  );
  int endPos = static_cast<int>(mImpl->stream.tellg());

  if (endPos == -1) {
    // error
    return 0;
  }

  return (endPos - startPos) / objectSize;
}
