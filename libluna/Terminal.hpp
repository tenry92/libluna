#pragma once

namespace Luna {
  /**
   * \brief Add support for special terminal functions such as changing the
   * color and initializing on video game consoles.
   */
  namespace Terminal {
    /**
     * \brief Available terminal colors.
     *
     * On Linux, there is only one brightness for the background, for example
     * red equals darkRed.
     *
     * The non-dark colors are bold on some Linux terminals.
     */
    enum Color : int {
      none = 0x0,
      black = 0x1,
      red = 0x2,
      brightRed = 0x3,
      green = 0x4,
      brightGreen = 0x5,
      yellow = 0x6,
      brightYellow = 0x7,
      blue = 0x8,
      brightBlue = 0x9,
      purple = 0xa,
      brightPurple = 0xb,
      cyan = 0xc,
      brightCyan = 0xd,
      gray = 0xe,
      brightGray = 0xf,
    };

    void init();

    void update();

    void quit();

    /**
     * \brief Set foreground and background color.
     */
    void setColor(Color fgColor, Color bgColor = black);

    /**
     * \brief Reset foreground and background color.
     */
    void resetColor();
  } // namespace Terminal
} // namespace Luna
