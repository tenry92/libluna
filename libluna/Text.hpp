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

    void setFont(Font *font);
    Font *getFont() const;

    /**
     * @name Priority
     *
     * Low values (negative) are drawn behind other drawables with higher values.
     */
    ///@{
    void setPriority(float priority);
    float getPriority() const;
    ///@}

    ///@{
    void setVisible(bool visible);
    bool isVisible() const;
    ///@}

    private:
    Font *mFont;
    String mContent;

    float mPriority{0};
    bool mVisible{true};
  };
} // namespace Luna
