#include <libluna/Text.hpp>

using namespace Luna;

Text::Text() = default;

Text::~Text() = default;

void Text::setContent(const String& content) { mContent = content; }

const String& Text::getContent() const { return mContent; }

void Text::setFont(Font* font) { mFont = font; }

Font* Text::getFont() const { return mFont; }

void Text::setSize(float size) { mSize = size; }

float Text::getSize() const { return mSize; }

void Text::setLineHeight(float lineHeight) { mLineHeight = lineHeight; }

float Text::getLineHeight() const { return mLineHeight; }
