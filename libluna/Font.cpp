#include <libluna/Font.hpp>

#include <map>

using namespace Luna;

int Font::getLineHeight() const { return mLineHeight; }

void Font::setLineHeight(int lineHeight) { mLineHeight = lineHeight; }

int Font::getBaseLine() const { return mBaseLine; }

void Font::setBaseLine(int baseLine) { mBaseLine = baseLine; }

Font::Glyph *Font::getGlyphByCodePoint(String::CodePoint codePoint) {
  return &mGlyphs.at(codePoint);
}

Font::Glyph *Font::makeGlyphForCodePoint(String::CodePoint codePoint) {
  mGlyphs.emplace(codePoint, Font::Glyph{});
  auto glyph = getGlyphByCodePoint(codePoint);
  glyph->codePoint = codePoint;

  return glyph;
}

const std::map<String::CodePoint, Font::Glyph> &Font::getGlyphs() const {
  return mGlyphs;
}
