#pragma once

#include <libluna/Font.hpp>
#include <libluna/String.hpp>

namespace Luna {
  class Text;
  using TextPtr = std::shared_ptr<Text>;

  class Text final {
    public:
    static TextPtr make();
    ~Text();

    void setContent(const Luna::String &content);
    const Luna::String &getContent() const;

    void setFont(FontResPtr font);
    FontResPtr getFont() const;

    private:
    Text();
    FontResPtr mFont;
    String mContent;
  };
} // namespace Luna
