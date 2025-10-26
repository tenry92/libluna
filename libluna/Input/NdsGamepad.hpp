#pragma once

namespace Luna::Input {
  /**
   * @brief Provides enum definitions for Nintendo DS gamepad buttons.
   *
   * @ingroup input
   */
  class NdsGamepad {
    public:
    enum Button {
      kA,
      kB,
      kX,
      kY,
      kL,
      kR,
      kStart,
      kSelect,
      kDpadUp,
      kDpadDown,
      kDpadLeft,
      kDpadRight,
    };
  };
}
