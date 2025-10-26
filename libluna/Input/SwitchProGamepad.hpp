#pragma once

namespace Luna::Input {
  /**
   * @brief Provides enum definitions for Nintendo Switch Pro gamepad buttons and axes.
   *
   * @ingroup input
   */
  class SwitchProGamepad {
    public:
    enum Button {
      kA,
      kB,
      kX,
      kY,

      kMinus,
      kPlus,
      kHome,

      kL,
      kR,
      kZL,
      kZR,

      kLeftStickButton,
      kRightStickButton,

      kDpadUp,
      kDpadDown,
      kDpadLeft,
      kDpadRight,

      kCapture, // Capture
    };

    enum Axis {
      kLeftStickX,
      kLeftStickY,
      kRightStickX,
      kRightStickY,
    };

    enum Stick {
      kLeftStick,
      kRightStick,
    };
  };
}
