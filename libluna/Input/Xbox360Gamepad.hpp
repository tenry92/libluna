#pragma once

namespace Luna::Input {
  /**
   * @brief Provides enum definitions for Xbox 360 gamepad buttons and axes.
   *
   * @ingroup input
   */
  class Xbox360Gamepad {
    public:
    enum Button {
      kA,
      kB,
      kX,
      kY,

      kBack,
      kStart,
      kGuide,

      kLeftBumper,
      kRightBumper,
      kLeftTrigger,
      kRightTrigger,

      kLeftStickButton,
      kRightStickButton,

      kDpadUp,
      kDpadDown,
      kDpadLeft,
      kDpadRight,
    };

    enum Axis {
      kLeftStickX,
      kLeftStickY,
      kRightStickX,
      kRightStickY,
      kLeftTriggerAxis,
      kRightTriggerAxis,
    };

    enum Stick {
      kLeftStick,
      kRightStick,
    };
  };
}
