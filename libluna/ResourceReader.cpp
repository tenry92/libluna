#include <libluna/ResourceReader.hpp>

#include <libluna/Application.hpp>
#include <libluna/Filesystem/FileReader.hpp>

using namespace Luna;

class ResourceReader::impl {
  public:
  std::unique_ptr<Filesystem::FileReader> fileReader;
};

ResourceReaderPtr ResourceReader::make(const char *name) {
  return ResourceReaderPtr(new ResourceReader(name));
}

ResourceReader::ResourceReader(const char *name)
    : mImpl{std::make_unique<impl>()} {
  auto filePath = Application::getInstance()->getAssetsPath().cd(name);
  mImpl->fileReader = Filesystem::FileReader::make(filePath);
}

ResourceReader::~ResourceReader() = default;

bool ResourceReader::isValid() const { return mImpl->fileReader->isValid(); }

std::size_t ResourceReader::getSize() const {
  return mImpl->fileReader->getSize();
}

bool ResourceReader::eof() const { return mImpl->fileReader->eof(); }

std::size_t ResourceReader::tell() { return mImpl->fileReader->tell(); }

std::size_t ResourceReader::seek(std::size_t position) {
  return mImpl->fileReader->seek(position);
}

std::size_t ResourceReader::seekRelative(int relativePosition) {
  return mImpl->fileReader->seekRelative(relativePosition);
}

std::size_t ResourceReader::read(
    uint8_t *buffer, std::size_t objectSize, std::size_t objectCount
) {
  return mImpl->fileReader->read(buffer, objectSize, objectCount);
}
