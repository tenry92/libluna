#include <libluna/Filesystem/Path.hpp>

#include <algorithm>
#include <stdexcept>

using namespace Luna::Filesystem;
using Luna::String;

class Path::impl {
  public:
  std::list<String> parts;
};

Path::Path() : mImpl{std::make_unique<impl>()} {}

Path::Path(const Path &other) : mImpl{std::make_unique<impl>()} {
  mImpl->parts = other.mImpl->parts;
}

Path::Path(const String &path) : mImpl{std::make_unique<impl>()} {
  auto uniformPath = path.replaceAll("\\", "/");
  /// @todo Remove multiple consequent slashes?
  uniformPath.split("/", std::back_inserter(mImpl->parts));
}

Path::~Path() = default;

Path Path::operator=(const Path &other) {
  mImpl->parts = other.mImpl->parts;

  return *this;
}

String Path::getRawPath() const {
  return String::join("/", mImpl->parts.cbegin(), mImpl->parts.cend());
}

String Path::basename() const { return mImpl->parts.back(); }

String Path::extname() const {
  auto parts = basename().split('.');

  if (parts.size() > 1 && parts.back().getLength() > 0) {
    return parts.back();
  }

  return "";
}

bool Path::isAbsolute() const {
  return (mImpl->parts.size() > 1 && mImpl->parts.front() == "") ||
         mImpl->parts.front().endsWith(":");
}

bool Path::isEmpty() const {
  return mImpl->parts.size() == 0 ||
         (mImpl->parts.size() == 1 && mImpl->parts.front() == "");
}

Path Path::cd(const Path &other) const {
  Path newPath;

  if (other.isAbsolute()) {
    newPath.mImpl->parts = other.mImpl->parts;
  } else {
    newPath.mImpl->parts = mImpl->parts;

    for (auto &&part : other.mImpl->parts) {
      if (part == ".") {
        continue;
      }

      if (part == "..") {
        if (newPath.mImpl->parts.size() > 0) {
          newPath.mImpl->parts.pop_back();
        }
      } else {
        newPath.mImpl->parts.push_back(part);
      }
    }
  }

  return newPath;
}

Path Path::up(std::size_t levels) const {
  Path newPath(*this);

  for (std::size_t i = 0; i < levels && newPath.mImpl->parts.size() > 0; ++i) {
    newPath.mImpl->parts.pop_back();
  }

  return newPath;
}
