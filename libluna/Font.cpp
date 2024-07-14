#include <libluna/Font.hpp>

#include <map>

using namespace Luna;

class Font::impl {
  public:
  int mLineHeight;
  int mBaseLine;

  std::map<String::CodePoint, Font::Char> mChars;
};

Font::Font() : mImpl{std::make_unique<impl>()} {}

FontPtr Font::make() {
  return FontPtr(new Font());
}

Font::~Font() = default;

int Font::getLineHeight() const {
  return mImpl->mLineHeight;
}

void Font::setLineHeight(int lineHeight) {
  mImpl->mLineHeight = lineHeight;
}

int Font::getBaseLine() const {
  return mImpl->mBaseLine;
}

void Font::setBaseLine(int baseLine) {
  mImpl->mBaseLine = baseLine;
}

Font::Char *Font::getCharByCodePoint(String::CodePoint codePoint) const {
  return &mImpl->mChars.at(codePoint);
}

Font::Char *Font::makeCharForCodePoint(String::CodePoint codePoint) {
  mImpl->mChars.emplace(codePoint, Font::Char {});
  return getCharByCodePoint(codePoint);
}
