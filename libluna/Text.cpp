#include <libluna/Text.hpp>

using namespace Luna;

Text::Text() = default;

Text::~Text() = default;

void Text::setContent(const String &content) { mContent = content; }

const String &Text::getContent() const { return mContent; }

void Text::setFont(Font *font) { mFont = font; }

Font *Text::getFont() const { return mFont; }

void Text::setPriority(float priority) { mPriority = priority; }

float Text::getPriority() const { return mPriority; }

void Text::setVisible(bool visible) { mVisible = visible; }

bool Text::isVisible() const { return mVisible; }
