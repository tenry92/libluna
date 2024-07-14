#pragma once

#include <memory>

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
    Luna::String &getContent() const;

    void setFont(FontResPtr font);
    FontResPtr getFont() const;

    private:
    Text();
    class impl;
    std::unique_ptr<impl> mImpl;
  };
}
