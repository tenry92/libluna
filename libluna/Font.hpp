#pragma once

#include <map>

#include <libluna/Image.hpp>
#include <libluna/Resource.hpp>
#include <libluna/String.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Font;
  using FontPtr = std::shared_ptr<Font>;
  using FontResPtr = ResourcePtr<Font>;

  class Font {
    public:
    struct Char {
      String::CodePoint codePoint;
      ImagePtr image;
      Vector2i offset;
      int advance;
    };

    static FontPtr make();

    ~Font();

    int getLineHeight() const;

    void setLineHeight(int lineHeight);

    int getBaseLine() const;

    void setBaseLine(int baseLine);

    Char *getCharByCodePoint(String::CodePoint codePoint);

    Char *makeCharForCodePoint(String::CodePoint codePoint);

    private:
    Font();
    int mLineHeight;
    int mBaseLine;

    std::map<String::CodePoint, Font::Char> mChars;
  };
} // namespace Luna
