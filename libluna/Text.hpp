#pragma once

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
  class Text final {
    public:
    Text();
    ~Text();

    void setContent(const Luna::String &content);
    const Luna::String &getContent() const;

    void setFont(FontResPtr font);
    FontResPtr getFont() const;

    private:
    FontResPtr mFont;
    String mContent;
  };
} // namespace Luna
