#pragma once

#include <libluna/Drawable2d.hpp>
#include <libluna/Font.hpp>
#include <libluna/String.hpp>

namespace Luna {
  class Text;
  using TextPtr = std::shared_ptr<Text>;

  /**
   * @brief A 2D text.
   *
   * @ingroup drawables2d
   */
  class Text final : public Drawable2d {
    public:
    Text();
    ~Text();

    void setContent(const String& content);
    const String& getContent() const;

    void setFont(Font* font);
    Font* getFont() const;

    void setSize(float size);
    float getSize() const;

    void setLineHeight(float lineHeight);
    float getLineHeight() const;

    private:
    Font* mFont;
    String mContent;
    float mSize{1.0f};
    float mLineHeight{1.0f};
  };
} // namespace Luna
