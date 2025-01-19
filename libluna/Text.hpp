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

    void setContent(const Luna::String &content);
    const Luna::String &getContent() const;

    void setFont(Font *font);
    Font *getFont() const;

    private:
    Font *mFont;
    String mContent;
  };
} // namespace Luna
