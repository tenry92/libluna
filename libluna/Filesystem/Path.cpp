#include <libluna/Filesystem/Path.hpp>

#include <algorithm>
#include <stdexcept>

using namespace Luna::Filesystem;
using Luna::String;

Path::Path() = default;

Path::Path(const Path &other) { mParts = other.mParts; }

Path::Path(const String &path) {
  auto uniformPath = path.replaceAll("\\", "/");
  /// @todo Remove multiple consequent slashes?
  uniformPath.split("/", std::back_inserter(mParts));
}

Path::~Path() = default;

Path Path::operator=(const Path &other) {
  mParts = other.mParts;

  return *this;
}

String Path::getRawPath() const {
  return String::join("/", mParts.cbegin(), mParts.cend());
}

String Path::basename() const { return mParts.back(); }

String Path::extname() const {
  auto parts = basename().split('.');

  if (parts.size() > 1 && parts.back().getLength() > 0) {
    return parts.back();
  }

  return "";
}

bool Path::isAbsolute() const {
  return (mParts.size() > 1 && mParts.front() == "") ||
         mParts.front().endsWith(":");
}

bool Path::isEmpty() const {
  return mParts.size() == 0 || (mParts.size() == 1 && mParts.front() == "");
}

Path Path::cd(const Path &other) const {
  Path newPath;

  if (other.isAbsolute()) {
    newPath.mParts = other.mParts;
  } else {
    newPath.mParts = mParts;

    for (auto &&part : other.mParts) {
      if (part == ".") {
        continue;
      }

      if (part == "..") {
        if (newPath.mParts.size() > 0) {
          newPath.mParts.pop_back();
        }
      } else {
        newPath.mParts.push_back(part);
      }
    }
  }

  return newPath;
}

Path Path::up(std::size_t levels) const {
  Path newPath(*this);

  for (std::size_t i = 0; i < levels && newPath.mParts.size() > 0; ++i) {
    newPath.mParts.pop_back();
  }

  return newPath;
}
