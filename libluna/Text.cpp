#include <libluna/Text.hpp>

using namespace Luna;

class Text::impl {
  public:
  FontResPtr mFont;
  String mContent;
};

TextPtr Text::make() {
  return TextPtr(new Text());
}

Text::Text() : mImpl{std::make_unique<impl>()} {}

Text::~Text() = default;

void Text::setContent(const String &content) {
  mImpl->mContent = content;
}

String &Text::getContent() const {
  return mImpl->mContent;
}

void Text::setFont(FontResPtr font) {
  mImpl->mFont = font;
}

FontResPtr Text::getFont() const {
  return mImpl->mFont;
}
