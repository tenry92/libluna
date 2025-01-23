#pragma once

#include <map>

#include <libluna/Image.hpp>
#include <libluna/ImageLoader.hpp>
#include <libluna/Rect.hpp>
#include <libluna/String.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Font;
  using FontPtr = std::shared_ptr<Font>;

  class Font {
    public:
    struct Glyph {
      String::CodePoint codePoint;
      ImageLoader* imageLoader;
      Recti crop;
      Vector2i offset;
      int advance;
    };

    int getLineHeight() const;

    void setLineHeight(int lineHeight);

    int getBaseLine() const;

    void setBaseLine(int baseLine);

    Glyph* getGlyphByCodePoint(String::CodePoint codePoint);

    Glyph* makeGlyphForCodePoint(String::CodePoint codePoint);

    const std::map<String::CodePoint, Glyph>& getGlyphs() const;

    private:
    int mLineHeight;
    int mBaseLine;

    std::map<String::CodePoint, Font::Glyph> mGlyphs;
  };
} // namespace Luna
