#include <libluna/Font.hpp>

#include <map>

using namespace Luna;

Font::Font() = default;

FontPtr Font::make() { return FontPtr(new Font()); }

Font::~Font() = default;

int Font::getLineHeight() const { return mLineHeight; }

void Font::setLineHeight(int lineHeight) { mLineHeight = lineHeight; }

int Font::getBaseLine() const { return mBaseLine; }

void Font::setBaseLine(int baseLine) { mBaseLine = baseLine; }

Font::Char *Font::getCharByCodePoint(String::CodePoint codePoint) {
  return &mChars.at(codePoint);
}

Font::Char *Font::makeCharForCodePoint(String::CodePoint codePoint) {
  mChars.emplace(codePoint, Font::Char{});
  auto ch = getCharByCodePoint(codePoint);
  ch->codePoint = codePoint;

  return ch;
}
