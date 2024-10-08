#include <libluna/ResourceReader.hpp>

#include <libluna/Application.hpp>
#include <libluna/Filesystem/FileReader.hpp>

using namespace Luna;

ResourceReaderPtr ResourceReader::make(const char *name) {
  return ResourceReaderPtr(new ResourceReader(name));
}

ResourceReader::ResourceReader(const char *name) {
  auto filePath = Application::getInstance()->getAssetsPath().cd(name);
  fileReader = Filesystem::FileReader::make(filePath);
}

ResourceReader::~ResourceReader() = default;

bool ResourceReader::isValid() const { return fileReader->isValid(); }

std::size_t ResourceReader::getSize() const {
  return fileReader->getSize();
}

bool ResourceReader::eof() const { return fileReader->eof(); }

std::size_t ResourceReader::tell() { return fileReader->tell(); }

std::size_t ResourceReader::seek(std::size_t position) {
  return fileReader->seek(position);
}

std::size_t ResourceReader::seekRelative(int relativePosition) {
  return fileReader->seekRelative(relativePosition);
}

std::size_t ResourceReader::read(
    uint8_t *buffer, std::size_t objectSize, std::size_t objectCount
) {
  return fileReader->read(buffer, objectSize, objectCount);
}
