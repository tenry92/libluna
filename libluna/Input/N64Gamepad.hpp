#pragma once

namespace Luna::Input {
  /**
   * @brief Provides enum definitions for Nintendo 64 gamepad buttons and axes.
   *
   * @ingroup input
   */
  class N64Gamepad {
    public:
    enum Button {
      kA,
      kB,
      kZ,
      kStart,
      kDpadUp,
      kDpadDown,
      kDpadLeft,
      kDpadRight,
      kL,
      kR,
      kCUp,
      kCDown,
      kCLeft,
      kCRight,
    };

    enum Axis {
      kStickX,
      kStickY,
    };
  };
}
